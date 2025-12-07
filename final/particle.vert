#version 410 core

// Particle data (for both particle heads and trail segments)
layout(location = 0) in vec3 orbit_center;        // Focus point (center of sphere)
layout(location = 1) in vec4 ellipse_params;      // (semi_major, semi_minor, mean_motion, phase)
layout(location = 2) in vec3 orbit_orientation;   // (inclination, azimuth, arg_periapsis)
layout(location = 3) in vec3 particle_color;
layout(location = 4) in vec4 trail_offsets;
layout(location = 5) in float segment_type;       // 0-0.9 = trail segment, 1 = particle head

// Output to fragment shader
out vec3 frag_color;
flat out int is_particle;
out float trail_fade;

// Uniforms
uniform mat4 model_matrix;
uniform mat4 projection_view_matrix;
uniform float point_size;
uniform float current_time;
uniform float min_distance;  // Sphere radius reference (used for validation)

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

// Solve Kepler's equation using Newton-Raphson iteration
// Returns eccentric anomaly E given mean anomaly M and eccentricity e
float solveKeplerEquation(float M, float e) {
    // Initial guess
    float E = M;
    
    // Newton-Raphson iterations (5 iterations is usually sufficient)
    for (int i = 0; i < 5; i++) {
        E = E - (E - e * sin(E) - M) / (1.0 - e * cos(E));
    }
    
    return E;
}

// Calculate particle position at a given time using proper Keplerian orbital mechanics
vec3 calculate_position(float time) {
    float a = ellipse_params.x;           // Semi-major axis
    float b = ellipse_params.y;           // Semi-minor axis
    float n = ellipse_params.z;           // Mean motion (angular velocity)
    float M0 = ellipse_params.w;          // Initial mean anomaly (phase)
    
    float inclination = orbit_orientation.x;      // Orbit tilt
    float azimuth = orbit_orientation.y;          // Longitude of ascending node
    float arg_periapsis = orbit_orientation.z;    // Argument of periapsis
    
    // Calculate eccentricity from semi-axes
    float e = sqrt(1.0 - (b * b) / (a * a));
    
    // Calculate mean anomaly at current time
    float M = M0 + n * time;
    M = mod(M, 6.28318530718);  // Wrap to [0, 2π]
    
    // Solve Kepler's equation for eccentric anomaly
    float E = solveKeplerEquation(M, e);
    
    // Convert eccentric anomaly to true anomaly (position in orbit)
    float true_anomaly = 2.0 * atan(
        sqrt((1.0 + e) / (1.0 - e)) * tan(E / 2.0)
    );
    
    // Calculate distance from focus (r = a(1 - e*cos(E)))
    float r = a * (1.0 - e * cos(E));
    
    // Position in orbital plane (focus at origin)
    vec3 orbit_pos = vec3(
        r * cos(true_anomaly),
        r * sin(true_anomaly),
        0.0
    );
    
    // Apply orbital rotations:
    // 1. Rotate by argument of periapsis (orients the ellipse in its plane)
    orbit_pos = rotateZ(arg_periapsis) * orbit_pos;
    
    // 2. Rotate by inclination (tilts the orbital plane)
    orbit_pos = rotateX(inclination) * orbit_pos;
    
    // 3. Rotate by azimuth (rotates around vertical axis)
    orbit_pos = rotateZ(azimuth) * orbit_pos;
    
    // Add to focus point (sphere center)
    vec3 position = orbit_center + orbit_pos;
    
    // Safety check: ensure orbit stays outside sphere (keeps min_distance from being optimized out)
    float dist_from_center = length(position);
    if (dist_from_center < min_distance * 0.99) {
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
        
        // Transform to clip space
        vec4 clip_pos = projection_view_matrix * model_matrix * vec4(position, 1.0);
        
        // Calculate perspective-correct point size using clip space w coordinate
        // The w coordinate contains the view-space depth
        // Dividing by w gives us proper perspective scaling
        float perspective_scale = 1.0 / clip_pos.w;
        
        // Scale the base point size by the perspective factor
        // Multiply by viewport-relative scale (adjust 800.0 to match your viewport height)
        gl_PointSize = point_size * perspective_scale * 800.0;
        
        // Clamp to reasonable range
        gl_PointSize = clamp(gl_PointSize, 2.0, 100.0);
        
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
