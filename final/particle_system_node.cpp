#include "final/particle_system_node.hpp"
#include <cmath>
#include <iostream>

namespace cg
{

ParticleSystemNode::ParticleSystemNode(const Point3& center, float sphere_radius, int initial_count)
    : sphere_radius_(sphere_radius)
    , point_size_(4.0f)
    , trails_enabled_(true)
    , trail_length_(0.2f)
    , vao_(0)
    , vbo_(0)
    , vbo_capacity_(0)
    , current_time_(0.0f)
{
    std::cout << "ParticleSystemNode constructor: sphere_radius=" << sphere_radius 
              << ", initial_count=" << initial_count << "\n";
    
    std::random_device rd;
    rng_.seed(rd());

    particles_.reserve(initial_count);
    for (int i = 0; i < initial_count; ++i)
    {
        Particle p;
        p.orbit_center = center;  // All particles orbit the same center
        init_particle(p);
        particles_.push_back(p);
    }
    
    std::cout << "  Created " << particles_.size() << " particles\n";
}

ParticleSystemNode::~ParticleSystemNode()
{
    cleanup_buffers();
}

bool ParticleSystemNode::get_locations()
{
    std::cout << "\n=== ParticleSystemNode::get_locations() ===\n";
    
    GLuint program = shader_program_.get_program();
    std::cout << "Shader program ID: " << program << "\n";
    
    if (program == 0) {
        std::cout << "ERROR: Shader program ID is 0! Shader didn't compile/link.\n";
        return false;
    }
    
    // Check if program is valid
    GLint link_status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    std::cout << "Program link status: " << (link_status ? "SUCCESS" : "FAILED") << "\n";
    
    if (!link_status) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0) {
            std::vector<char> log(log_length);
            glGetProgramInfoLog(program, log_length, nullptr, log.data());
            std::cout << "Program link error:\n" << log.data() << "\n";
        }
        return false;
    }
    
    // Hardcoded attribute locations matching the shader
    base_position_loc_ = 0;
    movement_params_loc_ = 1;
    noise_offsets_loc_ = 2;
    // Attribute 3 is particle_color (hardcoded in shader)
    trail_offsets_loc_ = 4;
    segment_type_loc_ = 5;
    
    std::cout << "\nAttribute locations (hardcoded):\n";
    std::cout << "  base_position_loc_   (layout 0): " << base_position_loc_ << "\n";
    std::cout << "  movement_params_loc_ (layout 1): " << movement_params_loc_ << "\n";
    std::cout << "  noise_offsets_loc_   (layout 2): " << noise_offsets_loc_ << "\n";
    std::cout << "  particle_color       (layout 3): 3 (hardcoded in shader)\n";
    std::cout << "  trail_offsets_loc_   (layout 4): " << trail_offsets_loc_ << "\n";
    std::cout << "  segment_type_loc_    (layout 5): " << segment_type_loc_ << "\n";
    
    std::cout << "\nQuerying uniform locations:\n";
    
    model_matrix_loc_ = glGetUniformLocation(program, "model_matrix");
    std::cout << "  model_matrix: " << model_matrix_loc_;
    if (model_matrix_loc_ < 0) std::cout << " [NOT FOUND]";
    std::cout << "\n";
    
    projection_view_matrix_loc_ = glGetUniformLocation(program, "projection_view_matrix");
    std::cout << "  projection_view_matrix: " << projection_view_matrix_loc_;
    if (projection_view_matrix_loc_ < 0) std::cout << " [NOT FOUND]";
    std::cout << "\n";
    
    point_size_loc_ = glGetUniformLocation(program, "point_size");
    std::cout << "  point_size: " << point_size_loc_;
    if (point_size_loc_ < 0) std::cout << " [NOT FOUND]";
    std::cout << "\n";
    
    current_time_loc_ = glGetUniformLocation(program, "current_time");
    std::cout << "  current_time: " << current_time_loc_;
    if (current_time_loc_ < 0) std::cout << " [NOT FOUND]";
    std::cout << "\n";
    
    min_distance_loc_ = glGetUniformLocation(program, "min_distance");
    std::cout << "  min_distance: " << min_distance_loc_;
    if (min_distance_loc_ < 0) std::cout << " [NOT FOUND]";
    std::cout << "\n";

    if (model_matrix_loc_ < 0 || projection_view_matrix_loc_ < 0 || 
        point_size_loc_ < 0 || current_time_loc_ < 0 || min_distance_loc_ < 0)
    {
        std::cout << "\nERROR: One or more uniform locations not found!\n";
        std::cout << "This usually means:\n";
        std::cout << "  1. Uniform name mismatch between shader and C++ code\n";
        std::cout << "  2. Uniform is declared but not used (gets optimized out)\n";
        std::cout << "  3. Shader compilation failed (check above for errors)\n";
        std::cout << "\nCheck that particle.vert contains all these uniforms:\n";
        std::cout << "  uniform mat4 model_matrix;\n";
        std::cout << "  uniform mat4 projection_view_matrix;\n";
        std::cout << "  uniform float point_size;\n";
        std::cout << "  uniform float current_time;\n";
        std::cout << "  uniform float min_distance;\n";
        return false;
    }

    std::cout << "\n✓ All uniform locations retrieved successfully!\n";
    std::cout << "==========================================\n\n";
    
    setup_buffers();
    return true;
}

void ParticleSystemNode::init_particle(Particle& p)
{
    // Orbital parameter distributions
    std::uniform_real_distribution<float> periapsis_dist(
        sphere_radius_ * 1.2f,    // Periapsis at least 20% beyond sphere surface
        sphere_radius_ * 2.0f     // Up to 2x sphere radius
    );
    
    std::uniform_real_distribution<float> eccentricity_dist(0.3f, 0.8f);  // Moderate to high eccentricity
    std::uniform_real_distribution<float> period_dist(3.0f, 8.0f);         // Orbital period in seconds
    std::uniform_real_distribution<float> phase_dist(0.0f, 2.0f * M_PI);   // Random starting position
    std::uniform_real_distribution<float> angle_dist(0.0f, M_PI);          // Inclination
    std::uniform_real_distribution<float> azimuth_dist(0.0f, 2.0f * M_PI); // Full 360° rotation
    
    // Color distributions for gray/asteroid appearance
    std::uniform_real_distribution<float> grey_dist(0.5f, 0.95f);
    std::uniform_real_distribution<float> tint_dist(-0.1f, 0.25f);

    // Calculate orbital elements
    float periapsis = periapsis_dist(rng_);         // Closest approach distance
    float e = eccentricity_dist(rng_);              // Eccentricity
    
    // Semi-major axis: a = periapsis / (1 - e)
    p.semi_major_axis = periapsis / (1.0f - e);
    
    // Semi-minor axis: b = a * sqrt(1 - e^2)
    p.semi_minor_axis = p.semi_major_axis * std::sqrt(1.0f - e * e);
    
    // Mean motion (average angular velocity): n = 2π / period
    float period = period_dist(rng_);
    p.mean_motion = (2.0f * M_PI) / period;
    
    // Random starting phase
    p.mean_anomaly_0 = phase_dist(rng_);
    
    // Random orbital orientation
    p.inclination = angle_dist(rng_);
    p.azimuth = azimuth_dist(rng_);
    p.arg_periapsis = azimuth_dist(rng_);  // Random orientation of periapsis in orbital plane

    // Gray/rocky color with slight variation
    float base_grey = grey_dist(rng_);
    p.color_r = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));
    p.color_g = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));
    p.color_b = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));
    
    // Trail offsets
    p.trail_offset_1 = trail_length_ * 0.25f;
    p.trail_offset_2 = trail_length_ * 0.50f;
    p.trail_offset_3 = trail_length_ * 0.75f;
    p.trail_offset_4 = trail_length_ * 1.00f;
}

void ParticleSystemNode::setup_buffers()
{
    if (particles_.empty()) {
        std::cout << "WARNING: setup_buffers() called with no particles!\n";
        return;
    }

    std::cout << "Setting up GPU buffers for " << particles_.size() << " particles...\n";

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    std::cout << "  Created VAO: " << vao_ << "\n";

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    std::cout << "  Created VBO: " << vbo_ << "\n";

    // Each "vertex" has 18 floats:
    // orbit_center(3), ellipse_params(4), orbit_orientation(3), 
    // particle_color(3), trail_offsets(4), segment_type(1)
    
    // We'll render: 5 trail vertices + 1 particle head = 6 vertices per particle
    vbo_capacity_ = particles_.size() * 6;
    size_t buffer_size = vbo_capacity_ * 18 * sizeof(float);
    std::cout << "  Allocating VBO: " << vbo_capacity_ << " vertices, " 
              << buffer_size << " bytes\n";
    
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

    // Attribute 0: orbit_center (vec3)
    glEnableVertexAttribArray(base_position_loc_);
    glVertexAttribPointer(base_position_loc_, 3, GL_FLOAT, GL_FALSE,
                         18 * sizeof(float), (void*)0);

    // Attribute 1: ellipse_params (vec4)
    glEnableVertexAttribArray(movement_params_loc_);
    glVertexAttribPointer(movement_params_loc_, 4, GL_FLOAT, GL_FALSE,
                         18 * sizeof(float), (void*)(3 * sizeof(float)));

    // Attribute 2: orbit_orientation (vec3)
    glEnableVertexAttribArray(noise_offsets_loc_);
    glVertexAttribPointer(noise_offsets_loc_, 3, GL_FLOAT, GL_FALSE,
                         18 * sizeof(float), (void*)(7 * sizeof(float)));

    // Attribute 3: particle_color (vec3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                         18 * sizeof(float), (void*)(10 * sizeof(float)));

    // Attribute 4: trail_offsets (vec4)
    glEnableVertexAttribArray(trail_offsets_loc_);
    glVertexAttribPointer(trail_offsets_loc_, 4, GL_FLOAT, GL_FALSE,
                         18 * sizeof(float), (void*)(13 * sizeof(float)));

    // Attribute 5: segment_type (float)
    glEnableVertexAttribArray(segment_type_loc_);
    glVertexAttribPointer(segment_type_loc_, 1, GL_FLOAT, GL_FALSE,
                         18 * sizeof(float), (void*)(17 * sizeof(float)));

    glBindVertexArray(0);
    
    std::cout << "  Uploading initial particle data...\n";
    upload_particle_data();
    std::cout << "  GPU buffers setup complete!\n";
}

void ParticleSystemNode::upload_particle_data()
{
    if (particles_.empty()) return;

    size_t needed_capacity = particles_.size() * 6;
    if (needed_capacity > vbo_capacity_)
    {
        vbo_capacity_ = needed_capacity * 2;
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 18 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        
        // Re-setup attributes
        glEnableVertexAttribArray(base_position_loc_);
        glVertexAttribPointer(base_position_loc_, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(movement_params_loc_);
        glVertexAttribPointer(movement_params_loc_, 4, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(noise_offsets_loc_);
        glVertexAttribPointer(noise_offsets_loc_, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (void*)(7 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (void*)(10 * sizeof(float)));
        glEnableVertexAttribArray(trail_offsets_loc_);
        glVertexAttribPointer(trail_offsets_loc_, 4, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (void*)(13 * sizeof(float)));
        glEnableVertexAttribArray(segment_type_loc_);
        glVertexAttribPointer(segment_type_loc_, 1, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (void*)(17 * sizeof(float)));
    }

    std::vector<float> vertex_data(particles_.size() * 6 * 18);
    
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        // For each particle, create 6 vertices:
        // - 5 trail segment vertices
        // - 1 particle head vertex
        
        for (int v = 0; v < 6; ++v)
        {
            size_t idx = (i * 6 + v) * 18;
            
            // orbit_center (3 floats)
            vertex_data[idx + 0] = particles_[i].orbit_center.x;
            vertex_data[idx + 1] = particles_[i].orbit_center.y;
            vertex_data[idx + 2] = particles_[i].orbit_center.z;
            
            // ellipse_params (4 floats): semi_major, semi_minor, mean_motion, mean_anomaly_0
            vertex_data[idx + 3] = particles_[i].semi_major_axis;
            vertex_data[idx + 4] = particles_[i].semi_minor_axis;
            vertex_data[idx + 5] = particles_[i].mean_motion;
            vertex_data[idx + 6] = particles_[i].mean_anomaly_0;
            
            // orbit_orientation (3 floats): inclination, azimuth, arg_periapsis
            vertex_data[idx + 7] = particles_[i].inclination;
            vertex_data[idx + 8] = particles_[i].azimuth;
            vertex_data[idx + 9] = particles_[i].arg_periapsis;
            
            // particle_color (3 floats)
            vertex_data[idx + 10] = particles_[i].color_r;
            vertex_data[idx + 11] = particles_[i].color_g;
            vertex_data[idx + 12] = particles_[i].color_b;
            
            // trail_offsets (4 floats)
            vertex_data[idx + 13] = particles_[i].trail_offset_1;
            vertex_data[idx + 14] = particles_[i].trail_offset_2;
            vertex_data[idx + 15] = particles_[i].trail_offset_3;
            vertex_data[idx + 16] = particles_[i].trail_offset_4;
            
            // segment_type (1 float)
            if (v < 4) {
                vertex_data[idx + 17] = v * 0.25f;  // Trail: 0.0, 0.25, 0.5, 0.75
            } else if (v == 4) {
                vertex_data[idx + 17] = 0.9f;  // Last trail point
            } else {
                vertex_data[idx + 17] = 1.0f;  // Particle head
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles_.size() * 6 * 18 * sizeof(float), vertex_data.data());
}

void ParticleSystemNode::draw(SceneState& scene_state)
{
    if (particles_.empty()) return;

    current_time_ += 1.0f / 60.0f;

    glUseProgram(shader_program_.get_program());

    glUniformMatrix4fv(model_matrix_loc_, 1, GL_FALSE, scene_state.model_matrix.get());
    glUniformMatrix4fv(projection_view_matrix_loc_, 1, GL_FALSE, scene_state.pv.get());
    glUniform1f(point_size_loc_, point_size_);
    glUniform1f(current_time_loc_, current_time_);
    glUniform1f(min_distance_loc_, sphere_radius_);

    glBindVertexArray(vao_);
    
    if (trails_enabled_)
    {
        // Enable blending for trails
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(2.0f);
        
        // Draw trail lines (first 5 vertices per particle as line strip)
        for (size_t i = 0; i < particles_.size(); ++i)
        {
            glDrawArrays(GL_LINE_STRIP, i * 6, 5);
        }
        
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }
    
    // Draw particle heads (6th vertex of each particle as point)
    glEnable(GL_PROGRAM_POINT_SIZE);
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        glDrawArrays(GL_POINTS, i * 6 + 5, 1);
    }
    
    glBindVertexArray(0);

    SceneNode::draw(scene_state);
}

void ParticleSystemNode::add_particles(int count)
{
    for (int i = 0; i < count; ++i)
    {
        Particle p;
        p.orbit_center = particles_.empty() ? Point3(0, 0, 0) : particles_[0].orbit_center;
        init_particle(p);
        particles_.push_back(p);
    }
    upload_particle_data();
    std::cout << "Added " << count << " particles. Total: " << particles_.size() << "\n";
}

void ParticleSystemNode::remove_particles(int count)
{
    int to_remove = std::min(count, static_cast<int>(particles_.size()));
    for (int i = 0; i < to_remove; ++i)
    {
        particles_.pop_back();
    }
    std::cout << "Removed " << to_remove << " particles. Total: " << particles_.size() << "\n";
}

void ParticleSystemNode::set_particle_size(float size)
{
    point_size_ = size;
}

void ParticleSystemNode::set_sphere_radius(float radius)
{
    sphere_radius_ = radius;
}

void ParticleSystemNode::set_trails_enabled(bool enabled)
{
    trails_enabled_ = enabled;
}

void ParticleSystemNode::set_trail_length(float length)
{
    trail_length_ = length;
    for (auto& p : particles_)
    {
        p.trail_offset_1 = trail_length_ * 0.25f;
        p.trail_offset_2 = trail_length_ * 0.50f;
        p.trail_offset_3 = trail_length_ * 0.75f;
        p.trail_offset_4 = trail_length_ * 1.00f;
    }
    upload_particle_data();
}

void ParticleSystemNode::cleanup_buffers()
{
    if (vbo_ != 0)
    {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0)
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
}

} // namespace cg
