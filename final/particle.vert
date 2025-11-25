#version 330 core

// Static particle parameters (uploaded once)
layout(location = 0) in vec3 base_position;    // Base position in swarm
layout(location = 1) in vec3 movement_params;  // x: speed, y: noise_scale, z: orbit_phase
layout(location = 2) in vec3 noise_offsets;    // Random offsets for noise sampling

// Uniforms
uniform mat4 pvm_matrix;
uniform float point_size;
uniform float current_time;
uniform float min_distance;  // Minimum distance from origin (sphere surface)

// Simple 3D noise function (hash-based)
float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

float noise3d(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    
    float n = p.x + p.y * 57.0 + 113.0 * p.z;
    return mix(
        mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
            mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
        mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
            mix(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}

void main()
{
    float speed = movement_params.x;
    float noise_scale = movement_params.y;
    float orbit_phase = movement_params.z;
    
    // Create animated time for this particle
    float t = current_time * speed + orbit_phase;
    
    // Sample 3D noise at different positions for x, y, z movement
    vec3 noise_sample_pos = vec3(t, t, t) * 0.5 + noise_offsets;
    
    float noise_x = noise3d(noise_sample_pos + vec3(0.0, 0.0, 0.0));
    float noise_y = noise3d(noise_sample_pos + vec3(10.5, 0.0, 0.0));
    float noise_z = noise3d(noise_sample_pos + vec3(0.0, 10.5, 0.0));
    
    // Convert noise from [0,1] to [-1,1]
    vec3 noise_offset = vec3(noise_x, noise_y, noise_z) * 2.0 - 1.0;
    
    // Apply noise to base position
    vec3 position = base_position + noise_offset * noise_scale;
    
    // SURFACE AVOIDANCE: Push particle out if it's too close to origin
    float dist = length(position);
    if (dist < min_distance) {
        // Push particle out to minimum distance along the direction from origin
        position = normalize(position) * min_distance;
    }
    
    gl_Position = pvm_matrix * vec4(position, 1.0);
    gl_PointSize = point_size;
}
