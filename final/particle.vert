#version 330 core

// Particle data (for both particle heads and trail segments)
layout(location = 0) in vec3 orbit_center;
layout(location = 1) in vec4 ellipse_params;
layout(location = 2) in vec3 orbit_orientation;
layout(location = 3) in vec3 particle_color;
layout(location = 4) in vec4 trail_offsets;
layout(location = 5) in float segment_type;  // 0 = trail segment, 1 = particle head

// Output to fragment shader
out vec3 frag_color;
flat out int is_particle;
out float trail_fade;

// Uniforms
uniform mat4 model_matrix;
uniform mat4 projection_view_matrix;
uniform float point_size;
uniform float current_time;
uniform float min_distance;

// Rotation matrices
mat3 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        1.0, 0.0, 0.0,
        0.0, c, -s,
        0.0, s, c
    );
}

mat3 rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, -s, 0.0,
        s, c, 0.0,
        0.0, 0.0, 1.0
    );
}

// Calculate particle position at a given time
vec3 calculate_position(float time) {
    float semi_major = ellipse_params.x;
    float semi_minor = ellipse_params.y;
    float orbital_speed = ellipse_params.z;
    float orbit_phase = ellipse_params.w;
    
    float inclination = orbit_orientation.x;
    float azimuth = orbit_orientation.y;
    
    float angle = time * orbital_speed + orbit_phase;
    
    vec3 orbit_pos = vec3(
        semi_major * cos(angle),
        semi_minor * sin(angle),
        0.0
    );
    
    orbit_pos = rotateZ(azimuth) * orbit_pos;
    orbit_pos = rotateX(inclination) * orbit_pos;
    
    vec3 position = orbit_center + orbit_pos;
    
    // Surface avoidance
    float dist = length(position);
    if (dist < min_distance) {
        position = normalize(position) * min_distance;
    }
    
    return position;
}

void main()
{
    // segment_type encoding:
    // 1.0 = particle head
    // 0.0 to 0.9 = trail segment (value indicates which segment and fade)
    
    vec3 position;
    
    if (segment_type >= 0.99) {
        // Render particle head
        position = calculate_position(current_time);
        is_particle = 1;
        trail_fade = 1.0;
        gl_PointSize = point_size;
    } else {
        // Render trail segment
        // segment_type encodes which trail point (0.0, 0.25, 0.5, 0.75)
        float time_offset;
        
        if (segment_type < 0.2) {
            time_offset = trail_offsets.w;  // Oldest
            trail_fade = 0.1;
        } else if (segment_type < 0.4) {
            time_offset = trail_offsets.z;
            trail_fade = 0.3;
        } else if (segment_type < 0.6) {
            time_offset = trail_offsets.y;
            trail_fade = 0.5;
        } else {
            time_offset = trail_offsets.x;  // Newest
            trail_fade = 0.7;
        }
        
        position = calculate_position(current_time - time_offset);
        is_particle = 0;
        gl_PointSize = 1.0;  // Not used for lines
    }
    
    gl_Position = projection_view_matrix * model_matrix * vec4(position, 1.0);
    frag_color = particle_color;
}
