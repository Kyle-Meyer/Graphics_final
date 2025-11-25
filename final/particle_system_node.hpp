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
    // Static parameters (set once, uploaded to GPU)
    float orbit_radius;      // Radius from orbit center
    float orbit_speed;       // Angular speed (radians per second)
    float phase_offset;      // Starting phase offset
    
};

/**
 * Particle system that creates a swarm of flies around a center point
 */
class ParticleSystemNode : public ShaderNode
{
  public:
    /**
     * Constructor.
     * @param center         Center point of the swarm (not used in local space)
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
    float swarm_radius_;

    // Particle appearance
    float particle_color_[3];  // RGB color
    float point_size_;

    // GPU resources
    GLuint vao_;
    GLuint vbo_;
    size_t vbo_capacity_;  // Current VBO capacity

    // Uniform and attribute locations
    GLint orbit_radius_loc_;
    GLint orbit_speed_loc_;
    GLint phase_offset_loc_;
    GLint pvm_matrix_loc_;
    GLint point_size_loc_;
    GLint particle_color_loc_;
    GLint current_time_loc_;

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
     * Upload particle data to GPU (called when particles are added/removed)
     */
    void upload_particle_data();

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
