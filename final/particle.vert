#version 330 core

// Static particle parameters (uploaded once)
layout(location = 0) in vec3 orbit_center;        // Center of elliptical orbit
layout(location = 1) in vec4 ellipse_params;      // x: semi_major_axis, y: semi_minor_axis, z: orbital_speed, w: orbit_phase
layout(location = 2) in vec3 orbit_orientation;   // x: inclination, y: azimuth, z: unused
layout(location = 3) in vec3 particle_color;      // Random color for this particle

// Output to fragment shader
out vec3 frag_color;

// Uniforms
uniform mat4 pvm_matrix;
uniform float point_size;
uniform float current_time;
uniform float min_distance;  // Minimum distance from origin (sphere surface)

// Rotation matrix around X axis
mat3 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        1.0, 0.0, 0.0,
        0.0, c, -s,
        0.0, s, c
    );
}

// Rotation matrix around Y axis
mat3 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, 0.0, s,
        0.0, 1.0, 0.0,
        -s, 0.0, c
    );
}

// Rotation matrix around Z axis
mat3 rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, -s, 0.0,
        s, c, 0.0,
        0.0, 0.0, 1.0
    );
}

void main()
{
    // Unpack ellipse parameters
    float semi_major = ellipse_params.x;
    float semi_minor = ellipse_params.y;
    float orbital_speed = ellipse_params.z;
    float orbit_phase = ellipse_params.w;

    // Unpack orbit orientation
    float inclination = orbit_orientation.x;
    float azimuth = orbit_orientation.y;

    // Calculate current angle in orbit
    float angle = current_time * orbital_speed + orbit_phase;

    // Calculate position on ellipse in 2D (XY plane)
    vec3 orbit_pos = vec3(
        semi_major * cos(angle),
        semi_minor * sin(angle),
        0.0
    );

    // Rotate orbit plane to create 3D orientation
    // First rotate around Z axis (azimuth)
    orbit_pos = rotateZ(azimuth) * orbit_pos;
    // Then rotate around X axis (inclination)
    orbit_pos = rotateX(inclination) * orbit_pos;

    // Add orbit center offset
    vec3 position = orbit_center + orbit_pos;

    // SURFACE AVOIDANCE: Push particle out if it's too close to origin
    float dist = length(position);
    if (dist < min_distance) {
        // Push particle out to minimum distance along the direction from origin
        position = normalize(position) * min_distance;
    }

    gl_Position = pvm_matrix * vec4(position, 1.0);
    gl_PointSize = point_size;

    // Pass color to fragment shader
    frag_color = particle_color;
}
