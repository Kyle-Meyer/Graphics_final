#include "final/particle_system_node.hpp"
#include <cmath>
#include <iostream>

namespace cg
{

ParticleSystemNode::ParticleSystemNode(const Point3& center, float swarm_radius, int initial_count)
    : swarm_radius_(swarm_radius)
    , particle_color_{0.0f, 0.0f, 0.0f}  // Black by default
    , point_size_(4.0f)
    , vao_(0)
    , vbo_(0)
    , vbo_capacity_(0)
    , current_time_(0.0f)
    , min_distance_(1.0f)
{
    // Initialize random number generator
    std::random_device rd;
    rng_.seed(rd());

    // Create initial particles
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
    // Get attribute locations (these match the layout locations in vertex shader)
    base_position_loc_ = 0;     // layout(location = 0) - vec3
    movement_params_loc_ = 1;   // layout(location = 1) - vec4
    noise_offsets_loc_ = 2;     // layout(location = 2) - vec3
    // Color is location 3 - vec3
    
    // Get uniform locations
    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    point_size_loc_ = glGetUniformLocation(shader_program_.get_program(), "point_size");
    current_time_loc_ = glGetUniformLocation(shader_program_.get_program(), "current_time");
    min_distance_loc_ = glGetUniformLocation(shader_program_.get_program(), "min_distance");

    if (pvm_matrix_loc_ < 0 || point_size_loc_ < 0 ||
        current_time_loc_ < 0 || min_distance_loc_ < 0)
    {
        std::cout << "Failed to get particle shader uniform locations\n";
        std::cout << "  pvm_matrix: " << pvm_matrix_loc_ << "\n";
        std::cout << "  point_size: " << point_size_loc_ << "\n";
        std::cout << "  current_time: " << current_time_loc_ << "\n";
        std::cout << "  min_distance: " << min_distance_loc_ << "\n";
        return false;
    }

    setup_buffers();
    return true;
}

void ParticleSystemNode::init_particle(Particle& p)
{
    // Random distribution generators
    std::uniform_real_distribution<float> pos_dist(-swarm_radius_ * 0.3f, swarm_radius_ * 0.3f);
    std::uniform_real_distribution<float> major_axis_dist(swarm_radius_ * 0.5f, swarm_radius_ * 1.5f);
    std::uniform_real_distribution<float> eccentricity_dist(0.3f, 0.9f);  // How elliptical (0=circle, 1=line)
    std::uniform_real_distribution<float> speed_dist(0.3f, 1.5f);
    std::uniform_real_distribution<float> phase_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> angle_dist(0.0f, M_PI);
    std::uniform_real_distribution<float> azimuth_dist(0.0f, 2.0f * M_PI);

    // Random yellowish-grey color (grey with slight yellow tint)
    std::uniform_real_distribution<float> grey_dist(0.5f, 0.95f);  // Base grey level
    std::uniform_real_distribution<float> tint_dist(-0.1f, 0.25f);  // Color tint variance

    // Random orbit center (slight offset from origin)
    p.orbit_center = Point3(
        pos_dist(rng_),
        pos_dist(rng_),
        pos_dist(rng_)
    );

    // Ellipse parameters
    p.semi_major_axis = major_axis_dist(rng_);
    float eccentricity = eccentricity_dist(rng_);
    p.semi_minor_axis = p.semi_major_axis * std::sqrt(1.0f - eccentricity * eccentricity);

    // Orbital motion parameters
    p.orbital_speed = speed_dist(rng_);
    p.orbit_phase = phase_dist(rng_);

    // Random orbit plane orientation
    p.inclination = angle_dist(rng_);
    p.azimuth = azimuth_dist(rng_);

    // Random grey with varied hue tints
    float base_grey = grey_dist(rng_);
    p.color_r = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));  // Random R tint
    p.color_g = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));  // Random G tint
    p.color_b = std::max(0.0f, std::min(1.0f, base_grey + tint_dist(rng_)));  // Random B tint
}

void ParticleSystemNode::setup_buffers()
{
    if (particles_.empty()) return;

    // Generate and bind VAO
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Generate and bind VBO
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Allocate initial buffer capacity
    vbo_capacity_ = particles_.size();

    // Each particle needs 13 floats:
    //   - orbit_center (3 floats)
    //   - semi_major_axis (1 float)
    //   - semi_minor_axis (1 float)
    //   - orbital_speed (1 float)
    //   - orbit_phase (1 float)
    //   - inclination (1 float)
    //   - azimuth (1 float)
    //   - color_r, color_g, color_b (3 floats)
    //   - padding (1 float)
    glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 13 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Set up vertex attribute pointers
    // Attribute 0: orbit_center (vec3)
    glEnableVertexAttribArray(base_position_loc_);
    glVertexAttribPointer(base_position_loc_, 3, GL_FLOAT, GL_FALSE,
                         13 * sizeof(float), (void*)0);

    // Attribute 1: ellipse_params (vec4: semi_major, semi_minor, orbital_speed, orbit_phase)
    glEnableVertexAttribArray(movement_params_loc_);
    glVertexAttribPointer(movement_params_loc_, 4, GL_FLOAT, GL_FALSE,
                         13 * sizeof(float), (void*)(3 * sizeof(float)));

    // Attribute 2: orbit_orientation (vec3: inclination, azimuth, unused)
    glEnableVertexAttribArray(noise_offsets_loc_);
    glVertexAttribPointer(noise_offsets_loc_, 3, GL_FLOAT, GL_FALSE,
                         13 * sizeof(float), (void*)(7 * sizeof(float)));

    // Attribute 3: particle_color (vec3: r, g, b)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                         13 * sizeof(float), (void*)(10 * sizeof(float)));

    glBindVertexArray(0);

    // Upload initial particle data
    upload_particle_data();
}



void ParticleSystemNode::resize_buffer_if_needed()
{
    if (particles_.size() > vbo_capacity_)
    {
        // Need to resize buffer
        vbo_capacity_ = particles_.size() * 2;  // Double capacity for growth

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 13 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        // Re-setup attribute pointers after buffer reallocation
        glEnableVertexAttribArray(base_position_loc_);
        glVertexAttribPointer(base_position_loc_, 3, GL_FLOAT, GL_FALSE,
                             13 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(movement_params_loc_);
        glVertexAttribPointer(movement_params_loc_, 4, GL_FLOAT, GL_FALSE,
                             13 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(noise_offsets_loc_);
        glVertexAttribPointer(noise_offsets_loc_, 3, GL_FLOAT, GL_FALSE,
                             13 * sizeof(float), (void*)(7 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                             13 * sizeof(float), (void*)(10 * sizeof(float)));
    }
}

void ParticleSystemNode::upload_particle_data()
{
    if (particles_.empty()) return;

    resize_buffer_if_needed();

    // Pack particle parameters into flat array
    std::vector<float> particle_data(particles_.size() * 13);
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        // Orbit center (3 floats)
        particle_data[i * 13 + 0] = particles_[i].orbit_center.x;
        particle_data[i * 13 + 1] = particles_[i].orbit_center.y;
        particle_data[i * 13 + 2] = particles_[i].orbit_center.z;

        // Ellipse params (4 floats: semi_major, semi_minor, orbital_speed, orbit_phase)
        particle_data[i * 13 + 3] = particles_[i].semi_major_axis;
        particle_data[i * 13 + 4] = particles_[i].semi_minor_axis;
        particle_data[i * 13 + 5] = particles_[i].orbital_speed;
        particle_data[i * 13 + 6] = particles_[i].orbit_phase;

        // Orbit orientation (3 floats: inclination, azimuth, unused padding)
        particle_data[i * 13 + 7] = particles_[i].inclination;
        particle_data[i * 13 + 8] = particles_[i].azimuth;
        particle_data[i * 13 + 9] = 0.0f;  // Unused padding

        // Particle color (3 floats: r, g, b)
        particle_data[i * 13 + 10] = particles_[i].color_r;
        particle_data[i * 13 + 11] = particles_[i].color_g;
        particle_data[i * 13 + 12] = particles_[i].color_b;
    }

    // Upload to GPU (this only happens when particles are added/removed, not every frame!)
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles_.size() * 13 * sizeof(float), particle_data.data());
}

void ParticleSystemNode::draw(SceneState& scene_state)
{
    if (particles_.empty()) return;

    // Update time (automatically advances animation)
    current_time_ += 1.0f / 60.0f;  // Advance by one frame at 60 FPS

    // Enable shader program
    glUseProgram(shader_program_.get_program());

    // Particles are in local space, so use full PVM matrix
    Matrix4x4 pvm = scene_state.pv * scene_state.model_matrix;
    glUniformMatrix4fv(pvm_matrix_loc_, 1, GL_FALSE, pvm.get());
    glUniform1f(point_size_loc_, point_size_);
    glUniform1f(current_time_loc_, current_time_);  // Send time to shader
    glUniform1f(min_distance_loc_, min_distance_);

    // Enable point sprites
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Draw particles
    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, particles_.size());
    glBindVertexArray(0);

    // Draw children (if any)
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
    
    // Re-upload all particle data to GPU
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

void ParticleSystemNode::set_min_distance(float distance)
{
    min_distance_ = distance;
}

} // namespace cg
