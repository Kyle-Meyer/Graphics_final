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
    orbit_radius_loc_ = 0;   // layout(location = 0)
    orbit_speed_loc_ = 1;    // layout(location = 1)
    phase_offset_loc_ = 2;   // layout(location = 2)
    
    // Get uniform locations
    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    point_size_loc_ = glGetUniformLocation(shader_program_.get_program(), "point_size");
    particle_color_loc_ = glGetUniformLocation(shader_program_.get_program(), "particle_color");
    current_time_loc_ = glGetUniformLocation(shader_program_.get_program(), "current_time");

    if (pvm_matrix_loc_ < 0 || point_size_loc_ < 0 || 
        particle_color_loc_ < 0 || current_time_loc_ < 0)
    {
        std::cout << "Failed to get particle shader uniform locations\n";
        std::cout << "  pvm_matrix: " << pvm_matrix_loc_ << "\n";
        std::cout << "  point_size: " << point_size_loc_ << "\n";
        std::cout << "  particle_color: " << particle_color_loc_ << "\n";
        std::cout << "  current_time: " << current_time_loc_ << "\n";
        return false;
    }

    setup_buffers();
    return true;
}

void ParticleSystemNode::init_particle(Particle& p)
{
    // Random distribution generators
    std::uniform_real_distribution<float> radius_dist(swarm_radius_ * 0.8f, swarm_radius_ * 1.2f);
    std::uniform_real_distribution<float> speed_dist(0.5f, 2.0f);
    std::uniform_real_distribution<float> phase_dist(0.0f, 2.0f * M_PI);
    
    // Set static parameters
    p.orbit_radius = radius_dist(rng_);
    p.orbit_speed = speed_dist(rng_);
    p.phase_offset = phase_dist(rng_);
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
    
    // Each particle needs 3 floats: orbit_radius, orbit_speed, phase_offset
    glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Set up vertex attribute pointers
    // Attribute 0: orbit_radius (1 float per particle)
    glEnableVertexAttribArray(orbit_radius_loc_);
    glVertexAttribPointer(orbit_radius_loc_, 1, GL_FLOAT, GL_FALSE, 
                         3 * sizeof(float), (void*)0);

    // Attribute 1: orbit_speed (1 float per particle)
    glEnableVertexAttribArray(orbit_speed_loc_);
    glVertexAttribPointer(orbit_speed_loc_, 1, GL_FLOAT, GL_FALSE, 
                         3 * sizeof(float), (void*)(sizeof(float)));

    // Attribute 2: phase_offset (1 float per particle)
    glEnableVertexAttribArray(phase_offset_loc_);
    glVertexAttribPointer(phase_offset_loc_, 1, GL_FLOAT, GL_FALSE, 
                         3 * sizeof(float), (void*)(2 * sizeof(float)));

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
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        
        // Re-setup attribute pointers after buffer reallocation
        glEnableVertexAttribArray(orbit_radius_loc_);
        glVertexAttribPointer(orbit_radius_loc_, 1, GL_FLOAT, GL_FALSE, 
                             3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(orbit_speed_loc_);
        glVertexAttribPointer(orbit_speed_loc_, 1, GL_FLOAT, GL_FALSE, 
                             3 * sizeof(float), (void*)(sizeof(float)));
        glEnableVertexAttribArray(phase_offset_loc_);
        glVertexAttribPointer(phase_offset_loc_, 1, GL_FLOAT, GL_FALSE, 
                             3 * sizeof(float), (void*)(2 * sizeof(float)));
    }
}

void ParticleSystemNode::upload_particle_data()
{
    if (particles_.empty()) return;

    resize_buffer_if_needed();

    // Pack particle parameters into flat array: [radius, speed, phase, radius, speed, phase, ...]
    std::vector<float> particle_data(particles_.size() * 3);
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        particle_data[i * 3 + 0] = particles_[i].orbit_radius;
        particle_data[i * 3 + 1] = particles_[i].orbit_speed;
        particle_data[i * 3 + 2] = particles_[i].phase_offset;
    }

    // Upload to GPU (this only happens when particles are added/removed, not every frame!)
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles_.size() * 3 * sizeof(float), particle_data.data());
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

} // namespace cg
