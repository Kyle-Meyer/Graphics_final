#version 330 core

// Static particle parameters (uploaded once)
layout(location = 0) in float orbit_radius;
layout(location = 1) in float orbit_speed;
layout(location = 2) in float phase_offset;

// Uniforms
uniform mat4 pvm_matrix;
uniform float point_size;
uniform float current_time;

void main()
{
    // Calculate current phase based on time
    float phase = phase_offset + orbit_speed * current_time;
    
    // Calculate position on equatorial orbit (XY plane, Z=0 in local space)
    vec3 position = vec3(
        orbit_radius * cos(phase),
        orbit_radius * sin(phase),
        0.0
    );
    
    gl_Position = pvm_matrix * vec4(position, 1.0);
    gl_PointSize = point_size;
}
