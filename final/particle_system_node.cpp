#include "final/particle_system_node.hpp"
#include <cmath>
#include <iostream>

namespace cg
{

ParticleSystemNode::ParticleSystemNode(const Point3& center, float swarm_radius, int initial_count)
    : swarm_radius_(swarm_radius)
    , particle_color_{0.0f, 0.0f, 0.0f}
    , point_size_(4.0f)
    , trails_enabled_(true)
    , trail_length_(0.2f)
    , vao_(0)
    , vbo_(0)
    , vbo_capacity_(0)
    , current_time_(0.0f)
    , min_distance_(1.0f)
{
    std::random_device rd;
    rng_.seed(rd());

    particles_.reserve(initial_count);
    for (int i = 0; i < initial_count; ++i)
    {
        Particle p;
        init_particle(p);
        particles_.push_back(p);
    }
}

ParticleSystemNode::~ParticleSystemNode()
{
    cleanup_buffers();
}

bool ParticleSystemNode::get_locations()
{
    base_position_loc_ = 0;
    movement_params_loc_ = 1;
    noise_offsets_loc_ = 2;
    trail_offsets_loc_ = 4;
    segment_type_loc_ = 5;
    
    model_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "model_matrix");
    projection_view_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "projection_view_matrix");
    point_size_loc_ = glGetUniformLocation(shader_program_.get_program(), "point_size");
    current_time_loc_ = glGetUniformLocation(shader_program_.get_program(), "current_time");
    min_distance_loc_ = glGetUniformLocation(shader_program_.get_program(), "min_distance");

    if (model_matrix_loc_ < 0 || projection_view_matrix_loc_ < 0 || 
        point_size_loc_ < 0 || current_time_loc_ < 0 || min_distance_loc_ < 0)
    {
        std::cout << "Failed to get particle shader uniform locations\n";
        return false;
    }

    setup_buffers();
    return true;
}

void ParticleSystemNode::init_particle(Particle& p)
{
    std::uniform_real_distribution<float> pos_dist(-swarm_radius_ * 0.3f, swarm_radius_ * 0.3f);
    std::uniform_real_distribution<float> major_axis_dist(swarm_radius_ * 0.5f, swarm_radius_ * 1.5f);
    std::uniform_real_distribution<float> eccentricity_dist(0.3f, 0.9f);
    std::uniform_real_distribution<float> speed_dist(0.3f, 1.5f);
    std::uniform_real_distribution<float> phase_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> angle_dist(0.0f, M_PI);
    std::uniform_real_distribution<float> azimuth_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> grey_dist(0.5f, 0.95f);
    std::uniform_real_distribution<float> tint_dist(-0.1f, 0.25f);

    p.orbit_center = Point3(pos_dist(rng_), pos_dist(rng_), pos_dist(rng_));
    p.semi_major_axis = major_axis_dist(rng_);
    float eccentricity = eccentricity_dist(rng_);
    p.semi_minor_axis = p.semi_major_axis * std::sqrt(1.0f - eccentricity * eccentricity);
    p.orbital_speed = speed_dist(rng_);
    p.orbit_phase = phase_dist(rng_);
    p.inclination = angle_dist(rng_);
    p.azimuth = azimuth_dist(rng_);

    float base_grey = grey_dist(rng_);
    p.color_r = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));
    p.color_g = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));
    p.color_b = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));
    
    p.trail_offset_1 = trail_length_ * 0.25f;
    p.trail_offset_2 = trail_length_ * 0.50f;
    p.trail_offset_3 = trail_length_ * 0.75f;
    p.trail_offset_4 = trail_length_ * 1.00f;
}

void ParticleSystemNode::setup_buffers()
{
    if (particles_.empty()) return;

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Each "vertex" has 18 floats:
    // orbit_center(3), ellipse_params(4), orbit_orientation(3), 
    // particle_color(3), trail_offsets(4), segment_type(1)
    
    // We'll render: 5 trail vertices + 1 particle head = 6 vertices per particle
    vbo_capacity_ = particles_.size() * 6;
    glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 18 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

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
    upload_particle_data();
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
        // - 5 trail segment vertices (with decreasing segment_type values)
        // - 1 particle head vertex (segment_type = 1.0)
        
        for (int v = 0; v < 6; ++v)
        {
            size_t idx = (i * 6 + v) * 18;
            
            // Same particle data for all 6 vertices
            vertex_data[idx + 0] = particles_[i].orbit_center.x;
            vertex_data[idx + 1] = particles_[i].orbit_center.y;
            vertex_data[idx + 2] = particles_[i].orbit_center.z;
            vertex_data[idx + 3] = particles_[i].semi_major_axis;
            vertex_data[idx + 4] = particles_[i].semi_minor_axis;
            vertex_data[idx + 5] = particles_[i].orbital_speed;
            vertex_data[idx + 6] = particles_[i].orbit_phase;
            vertex_data[idx + 7] = particles_[i].inclination;
            vertex_data[idx + 8] = particles_[i].azimuth;
            vertex_data[idx + 9] = 0.0f;
            vertex_data[idx + 10] = particles_[i].color_r;
            vertex_data[idx + 11] = particles_[i].color_g;
            vertex_data[idx + 12] = particles_[i].color_b;
            vertex_data[idx + 13] = particles_[i].trail_offset_1;
            vertex_data[idx + 14] = particles_[i].trail_offset_2;
            vertex_data[idx + 15] = particles_[i].trail_offset_3;
            vertex_data[idx + 16] = particles_[i].trail_offset_4;
            
            // segment_type: 0.0-0.75 for trail segments, 1.0 for particle head
            if (v < 4) {
                vertex_data[idx + 17] = v * 0.25f;  // Trail segments: 0.0, 0.25, 0.5, 0.75
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
    glUniform1f(min_distance_loc_, min_distance_);

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

void ParticleSystemNode::set_particle_color(float r, float g, float b)
{
    particle_color_[0] = r;
    particle_color_[1] = g;
    particle_color_[2] = b;
}

void ParticleSystemNode::set_particle_size(float size)
{
    point_size_ = size;
}

void ParticleSystemNode::set_min_distance(float distance)
{
    min_distance_ = distance;
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
