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

    // Create single particle that will orbit at equator
    particles_.reserve(1);
    Particle p;
    init_particle(p);
    particles_.push_back(p);
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
    // Place single sphere on equator at fixed radius in local space
    float r = swarm_radius_;

    // Start at angle 0 on the equator (XY plane, z=0) in local space
    p.position = Point3(r, 0.0f, 0.0f);

    // Orbit parameters for equatorial rotation (around local origin)
    p.orbit_center = Point3(0.0f, 0.0f, 0.0f);
    p.orbit_radius = r;
    p.orbit_speed = 1.0f;  // Radians per second
    p.phase = 0.0f;        // Starting phase
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

        // Update phase for circular orbit
        p.phase += p.orbit_speed * delta_time;
        if (p.phase > 2.0f * M_PI)
            p.phase -= 2.0f * M_PI;

        // Calculate position on equatorial circle (XY plane, z=0) in local space
        p.position = Point3(
            p.orbit_radius * std::cos(p.phase),
            p.orbit_radius * std::sin(p.phase),
            0.0f  // Keep at equator (z=0)
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

    // Debug: Print model matrix to verify it has the sphere's transform
    static int debug_count = 0;
    if (debug_count++ % 60 == 0)
    {
        std::cout << "Model matrix translation (should be sphere position): ("
                  << scene_state.model_matrix.m03() << ", "
                  << scene_state.model_matrix.m13() << ", "
                  << scene_state.model_matrix.m23() << ")\n";
    }

    // Particles are in local space, so use full PVM matrix to transform with the primitive
    Matrix4x4 pvm = scene_state.pv * scene_state.model_matrix;
    glUniformMatrix4fv(pvm_matrix_loc_, 1, GL_FALSE, pvm.get());
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
