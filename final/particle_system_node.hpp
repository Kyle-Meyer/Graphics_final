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
 * Particle structure for Keplerian elliptical orbit behavior with trail support
 */
struct Particle
{
    // Orbital elements
    Point3 orbit_center;     // Focus of the ellipse (center of sphere being orbited)
    float semi_major_axis;   // Semi-major axis length (a)
    float semi_minor_axis;   // Semi-minor axis length (b)
    float mean_motion;       // Mean angular velocity (n = 2π/period)
    float mean_anomaly_0;    // Initial mean anomaly (phase offset)
    
    // Orbital orientation (Euler angles)
    float inclination;       // Tilt of orbit plane (i) - rotation about X
    float azimuth;           // Longitude of ascending node (Ω) - rotation about Z
    float arg_periapsis;     // Argument of periapsis (ω) - rotation about Z in orbital plane
    
    // Visual properties
    float color_r;           // Red component
    float color_g;           // Green component
    float color_b;           // Blue component
    
    // Trail time offsets (how far back in time each trail segment is)
    float trail_offset_1;    // Usually ~0.05 seconds back
    float trail_offset_2;    // Usually ~0.10 seconds back
    float trail_offset_3;    // Usually ~0.15 seconds back
    float trail_offset_4;    // Usually ~0.20 seconds back
};

/**
 * Particle system that creates particles in proper Keplerian elliptical orbits with GPU-computed trails
 */
class ParticleSystemNode : public ShaderNode
{
  public:
    /**
     * Constructor.
     * @param center         Center point of the sphere being orbited (typically origin)
     * @param sphere_radius  Radius of the sphere (periapsis will be outside this)
     * @param initial_count  Initial number of particles
     */
    ParticleSystemNode(const Point3& center = Point3(0.0f, 0.0f, 0.0f),
                       float sphere_radius = 1.0f,
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
     * Add more particles to the system
     * @param count  Number of particles to add
     */
    void add_particles(int count);

    /**
     * Remove particles from the system
     * @param count  Number of particles to remove
     */
    void remove_particles(int count);

    /**
     * Get current particle count
     */
    int get_particle_count() const { return particles_.size(); }

    /**
     * Get current animation time
     */
    float get_current_time() const { return current_time_; }

    /**
     * Set the size of particles
     * @param size  Point size in pixels
     */
    void set_particle_size(float size);

    /**
     * Set the radius of the central sphere being orbited
     * @param radius  Sphere radius (orbits will be outside this)
     */
    void set_sphere_radius(float radius);
    
    /**
     * Enable or disable particle trails
     * @param enabled  True to show trails, false to hide
     */
    void set_trails_enabled(bool enabled);
    
    /**
     * Set the trail length (time duration)
     * @param length  Trail length in seconds
     */
    void set_trail_length(float length);

  protected:
    // Particle data
    std::vector<Particle> particles_;
    float sphere_radius_;    // Radius of central sphere
    
    // Particle appearance
    float point_size_;
    
    // Trail settings
    bool trails_enabled_;
    float trail_length_;     // Total length of trail in seconds

    // GPU resources
    GLuint vao_;
    GLuint vbo_;
    size_t vbo_capacity_;    // Current VBO capacity

    // Uniform and attribute locations
    GLint base_position_loc_;
    GLint movement_params_loc_;
    GLint noise_offsets_loc_;
    GLint trail_offsets_loc_;
    GLint segment_type_loc_;
    GLint model_matrix_loc_;
    GLint projection_view_matrix_loc_;
    GLint point_size_loc_;
    GLint current_time_loc_;
    GLint min_distance_loc_;

    // Time tracking
    float current_time_;

    // Random number generator
    std::mt19937 rng_;

    /**
     * Initialize a single particle with Keplerian orbital elements
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
