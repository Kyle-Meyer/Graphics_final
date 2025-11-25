#ifndef __FINAL_PARTICLE_SYSTEM_NODE_HPP__
#define __FINAL_PARTICLE_SYSTEM_NODE_HPP__

#include "scene/shader_node.hpp"
#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"
#include <vector>
#include <random>

namespace cg
{

/**
 * Particle structure for 3D swarm behavior
 */
struct Particle
{
    Point3 position;         // Current position in world space
    Vector3 velocity;        // Current velocity
    Point3 orbit_center;     // Individual orbit center
    float orbit_radius;      // Radius from orbit center
    float orbit_speed;       // Angular speed
    float phase;             // Current phase in orbit
    float vertical_offset;   // Z-axis wobble offset
};

/**
 * Particle system that creates a swarm of flies around a center point
 */
class ParticleSystemNode : public ShaderNode
{
  public:
    /**
     * Constructor.
     * @param center         Center point of the swarm
     * @param swarm_radius   Radius of the swarm area
     * @param initial_count  Initial number of particles
     */
    ParticleSystemNode(const Point3& center = Point3(0.0f, 0.0f, 0.0f),
                       float swarm_radius = 15.0f,
                       int initial_count = 50);

    /**
     * Destructor - cleans up GPU resources.
     */
    virtual ~ParticleSystemNode();

    /**
     * Gets uniform and attribute locations.
     */
    bool get_locations() override;

    /**
     * Draw method - updates particles and renders them
     * @param  scene_state   Current scene state.
     */
    void draw(SceneState &scene_state) override;

    /**
     * Update particle positions based on time
     * @param delta_time  Time elapsed since last update (in seconds)
     */
    void update(float delta_time);

    /**
     * Add more particles to the swarm
     * @param count  Number of particles to add
     */
    void add_particles(int count);

    /**
     * Remove particles from the swarm
     * @param count  Number of particles to remove
     */
    void remove_particles(int count);

    /**
     * Get current particle count
     */
    int get_particle_count() const { return particles_.size(); }

    /**
     * Set the color of particles
     * @param r  Red component (0-1)
     * @param g  Green component (0-1)
     * @param b  Blue component (0-1)
     */
    void set_particle_color(float r, float g, float b);

    /**
     * Set the size of particles
     * @param size  Point size in pixels
     */
    void set_particle_size(float size);

  protected:
    // Particle data
    std::vector<Particle> particles_;
    Point3 swarm_center_;
    float swarm_radius_;

    // Particle appearance
    float particle_color_[3];  // RGB color
    float point_size_;

    // GPU resources
    GLuint vao_;
    GLuint vbo_;
    size_t vbo_capacity_;  // Current VBO capacity

    // Uniform and attribute locations
    GLint position_loc_;
    GLint pvm_matrix_loc_;
    GLint point_size_loc_;
    GLint particle_color_loc_;

    // Time tracking
    float current_time_;

    // Random number generator
    std::mt19937 rng_;

    /**
     * Initialize a single particle
     */
    void init_particle(Particle& p);

    /**
     * Setup GPU buffers
     */
    void setup_buffers();

    /**
     * Update GPU buffer with current particle positions
     */
    void update_buffer();

    /**
     * Resize GPU buffer if needed
     */
    void resize_buffer_if_needed();

    /**
     * Clean up GPU resources
     */
    void cleanup_buffers();
};

} // namespace cg

#endif
