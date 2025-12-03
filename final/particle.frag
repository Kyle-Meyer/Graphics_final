#version 330 core

in vec3 frag_color;

out vec4 fragColor;

void main()
{
    // Create circular particles by discarding fragments outside a circle
    vec2 coord = gl_PointCoord - vec2(0.5);
    if (length(coord) > 0.5)
        discard;

    fragColor = vec4(frag_color, 1.0);
}


