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
    movement_params_loc_ = 1;   // layout(location = 1) - vec3
    noise_offsets_loc_ = 2;     // layout(location = 2) - vec3
    
    // Get uniform locations
    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    point_size_loc_ = glGetUniformLocation(shader_program_.get_program(), "point_size");
    particle_color_loc_ = glGetUniformLocation(shader_program_.get_program(), "particle_color");
    current_time_loc_ = glGetUniformLocation(shader_program_.get_program(), "current_time");
    min_distance_loc_ = glGetUniformLocation(shader_program_.get_program(), "min_distance");

    if (pvm_matrix_loc_ < 0 || point_size_loc_ < 0 || 
        particle_color_loc_ < 0 || current_time_loc_ < 0 || min_distance_loc_ < 0)
    {
        std::cout << "Failed to get particle shader uniform locations\n";
        std::cout << "  pvm_matrix: " << pvm_matrix_loc_ << "\n";
        std::cout << "  point_size: " << point_size_loc_ << "\n";
        std::cout << "  particle_color: " << particle_color_loc_ << "\n";
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
    std::uniform_real_distribution<float> pos_dist(-swarm_radius_, swarm_radius_);
    std::uniform_real_distribution<float> speed_dist(0.3f, 1.5f);  // Flies move at varying speeds
    std::uniform_real_distribution<float> noise_scale_dist(0.3f, 0.8f);  // How erratic they are
    std::uniform_real_distribution<float> phase_dist(0.0f, 100.0f);
    std::uniform_real_distribution<float> noise_offset_dist(0.0f, 100.0f);
    
    // Random base position within spherical volume
    // Use uniform distribution in sphere
    float theta = pos_dist(rng_) * M_PI;  // 0 to PI
    float phi = pos_dist(rng_) * M_PI;    // -PI to PI
    float r = std::cbrt(std::uniform_real_distribution<float>(0.0f, 1.0f)(rng_)) * swarm_radius_;
    
    p.base_position = Point3(
        r * std::sin(theta) * std::cos(phi),
        r * std::sin(theta) * std::sin(phi),
        r * std::cos(theta)
    );
    
    // Movement parameters
    p.speed = speed_dist(rng_);
    p.noise_scale = noise_scale_dist(rng_) * swarm_radius_;
    p.orbit_phase = phase_dist(rng_);
    
    // Random noise offsets for variation
    p.noise_offsets = Vector3(
        noise_offset_dist(rng_),
        noise_offset_dist(rng_),
        noise_offset_dist(rng_)
    );
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
    
    // Each particle needs 9 floats: 
    //   - base_position (3 floats)
    //   - movement_params (3 floats: speed, noise_scale, orbit_phase)
    //   - noise_offsets (3 floats)
    glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 9 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Set up vertex attribute pointers
    // Attribute 0: base_position (vec3)
    glEnableVertexAttribArray(base_position_loc_);
    glVertexAttribPointer(base_position_loc_, 3, GL_FLOAT, GL_FALSE, 
                         9 * sizeof(float), (void*)0);

    // Attribute 1: movement_params (vec3: speed, noise_scale, orbit_phase)
    glEnableVertexAttribArray(movement_params_loc_);
    glVertexAttribPointer(movement_params_loc_, 3, GL_FLOAT, GL_FALSE, 
                         9 * sizeof(float), (void*)(3 * sizeof(float)));

    // Attribute 2: noise_offsets (vec3)
    glEnableVertexAttribArray(noise_offsets_loc_);
    glVertexAttribPointer(noise_offsets_loc_, 3, GL_FLOAT, GL_FALSE, 
                         9 * sizeof(float), (void*)(6 * sizeof(float)));

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
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 9 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        
        // Re-setup attribute pointers after buffer reallocation
        glEnableVertexAttribArray(base_position_loc_);
        glVertexAttribPointer(base_position_loc_, 3, GL_FLOAT, GL_FALSE, 
                             9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(movement_params_loc_);
        glVertexAttribPointer(movement_params_loc_, 3, GL_FLOAT, GL_FALSE, 
                             9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(noise_offsets_loc_);
        glVertexAttribPointer(noise_offsets_loc_, 3, GL_FLOAT, GL_FALSE, 
                             9 * sizeof(float), (void*)(6 * sizeof(float)));
    }
}

void ParticleSystemNode::upload_particle_data()
{
    if (particles_.empty()) return;

    resize_buffer_if_needed();

    // Pack particle parameters into flat array
    std::vector<float> particle_data(particles_.size() * 9);
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        // Base position (3 floats)
        particle_data[i * 9 + 0] = particles_[i].base_position.x;
        particle_data[i * 9 + 1] = particles_[i].base_position.y;
        particle_data[i * 9 + 2] = particles_[i].base_position.z;
        
        // Movement params (3 floats: speed, noise_scale, orbit_phase)
        particle_data[i * 9 + 3] = particles_[i].speed;
        particle_data[i * 9 + 4] = particles_[i].noise_scale;
        particle_data[i * 9 + 5] = particles_[i].orbit_phase;
        
        // Noise offsets (3 floats)
        particle_data[i * 9 + 6] = particles_[i].noise_offsets.x;
        particle_data[i * 9 + 7] = particles_[i].noise_offsets.y;
        particle_data[i * 9 + 8] = particles_[i].noise_offsets.z;
    }

    // Upload to GPU (this only happens when particles are added/removed, not every frame!)
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles_.size() * 9 * sizeof(float), particle_data.data());
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
    glUniform3fv(particle_color_loc_, 1, particle_color_);
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
    
    std::cout << "Added " << count << " flies. Total: " << particles_.size() << "\n";
}

void ParticleSystemNode::remove_particles(int count)
{
    int to_remove = std::min(count, static_cast<int>(particles_.size()));
    for (int i = 0; i < to_remove; ++i)
    {
        particles_.pop_back();
    }
    
    std::cout << "Removed " << to_remove << " flies. Total: " << particles_.size() << "\n";
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
