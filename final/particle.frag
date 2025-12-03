#version 330 core

in vec3 frag_color;
flat in int is_particle;
in float trail_fade;

out vec4 fragColor;

void main()
{
    if (is_particle == 1) {
        // Render particle head as circular point sprite
        vec2 coord = gl_PointCoord - vec2(0.5);
        if (length(coord) > 0.5)
            discard;
        
        fragColor = vec4(frag_color, 1.0);
    } else {
        // Render trail segment with alpha fade
        fragColor = vec4(frag_color, trail_fade);
    }
}


