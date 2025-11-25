#include "final/particle_system_node.hpp"
#include <cmath>
#include <iostream>

namespace cg
{

ParticleSystemNode::ParticleSystemNode(const Point3& center, float swarm_radius, int initial_count)
    : swarm_center_(center)
    , swarm_radius_(swarm_radius)
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
    position_loc_ = glGetAttribLocation(shader_program_.get_program(), "position");
    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    point_size_loc_ = glGetUniformLocation(shader_program_.get_program(), "point_size");
    particle_color_loc_ = glGetUniformLocation(shader_program_.get_program(), "particle_color");

    if (position_loc_ < 0 || pvm_matrix_loc_ < 0 || point_size_loc_ < 0 || particle_color_loc_ < 0)
    {
        std::cout << "Failed to get particle shader locations\n";
        return false;
    }

    setup_buffers();
    return true;
}

void ParticleSystemNode::init_particle(Particle& p)
{
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radius_dist(5.0f, 15.0f);

    // Simple: place particles randomly around swarm center
    float theta = angle_dist(rng_);
    float phi = angle_dist(rng_);
    float r = radius_dist(rng_);

    // Direct world position - no complex orbit centers
    p.position = swarm_center_ + Vector3(
        r * std::sin(phi) * std::cos(theta),
        r * std::sin(phi) * std::sin(theta),
        r * std::cos(phi)
    );

    // Simple orbit parameters
    p.orbit_center = swarm_center_;
    p.orbit_radius = r;
    p.orbit_speed = 1.0f;
    p.phase = theta;
    p.vertical_offset = 0.0f;
    p.velocity = Vector3(0.0f, 0.0f, 0.0f);
}

void ParticleSystemNode::setup_buffers()
{
    // Generate and bind VAO
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Generate and bind VBO
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Allocate initial buffer
    vbo_capacity_ = particles_.size();
    glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Set up vertex attribute pointer
    glEnableVertexAttribArray(position_loc_);
    glVertexAttribPointer(position_loc_, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
}

void ParticleSystemNode::resize_buffer_if_needed()
{
    if (particles_.size() > vbo_capacity_)
    {
        // Need to resize buffer
        vbo_capacity_ = particles_.size() * 2;  // Double capacity for growth

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity_ * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    }
}

void ParticleSystemNode::update(float delta_time)
{
    current_time_ += delta_time;

    for (size_t i = 0; i < particles_.size(); ++i)
    {
        Particle& p = particles_[i];

        // Simple circular orbit around swarm center
        p.phase += p.orbit_speed * delta_time;
        if (p.phase > 2.0f * M_PI)
            p.phase -= 2.0f * M_PI;

        // Calculate position on spherical shell around swarm center
        float vertical_phase = p.phase * 0.7f;

        p.position = swarm_center_ + Vector3(
            p.orbit_radius * std::sin(vertical_phase) * std::cos(p.phase),
            p.orbit_radius * std::sin(vertical_phase) * std::sin(p.phase),
            p.orbit_radius * std::cos(vertical_phase)
        );
    }
}

void ParticleSystemNode::update_buffer()
{
    if (particles_.empty()) return;

    resize_buffer_if_needed();

    // Extract positions into flat array
    std::vector<float> positions(particles_.size() * 3);
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        positions[i * 3 + 0] = particles_[i].position.x;
        positions[i * 3 + 1] = particles_[i].position.y;
        positions[i * 3 + 2] = particles_[i].position.z;
    }

    // Debug: print first particle position
    if (particles_.size() > 0)
    {
        static int debug_counter = 0;
        if (debug_counter++ % 60 == 0)  // Print once per second
        {
            std::cout << "Particle 0 pos: (" << particles_[0].position.x << ", "
                      << particles_[0].position.y << ", " << particles_[0].position.z << ")\n";
        }
    }

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles_.size() * 3 * sizeof(float), positions.data());
}

void ParticleSystemNode::draw(SceneState& scene_state)
{
    if (particles_.empty()) return;

    // Update particle positions
    update(1.0f / 60.0f);  // Fixed timestep for 60 FPS

    // Update GPU buffer with new positions
    update_buffer();

    // Enable shader program
    glUseProgram(shader_program_.get_program());

    // Debug: Print matrix info once
    static bool printed = false;
    if (!printed)
    {
        std::cout << "\n=== PARTICLE SYSTEM MATRIX DEBUG ===\n";
        std::cout << "scene_state.pv first row: " << scene_state.pv.m00() << ", "
                  << scene_state.pv.m01() << ", " << scene_state.pv.m02() << ", "
                  << scene_state.pv.m03() << "\n";
        std::cout << "scene_state.model_matrix first row: " << scene_state.model_matrix.m00() << ", "
                  << scene_state.model_matrix.m01() << ", " << scene_state.model_matrix.m02() << ", "
                  << scene_state.model_matrix.m03() << "\n";
        std::cout << "====================================\n\n";
        printed = true;
    }

    // Particles are already in world space, so only use projection-view matrix
    // Don't multiply by model_matrix since that would transform them again
    glUniformMatrix4fv(pvm_matrix_loc_, 1, GL_TRUE, scene_state.pv.get());
    glUniform1f(point_size_loc_, point_size_);
    glUniform3fv(particle_color_loc_, 1, particle_color_);

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
}

void ParticleSystemNode::remove_particles(int count)
{
    int to_remove = std::min(count, static_cast<int>(particles_.size()));
    for (int i = 0; i < to_remove; ++i)
    {
        particles_.pop_back();
    }
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
