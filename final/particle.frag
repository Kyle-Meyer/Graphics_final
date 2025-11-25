#version 330 core

out vec4 fragColor;

uniform vec3 particle_color;

void main()
{
    // Create circular particles by discarding fragments outside a circle
    vec2 coord = gl_PointCoord - vec2(0.5);
    if (length(coord) > 0.5)
        discard;

    fragColor = vec4(particle_color, 1.0);
}


