#version 410 core

// Trail shader with fading effect based on age

in float v_alpha;

out vec4 frag_color;

void main()
{
    // Grey color that fades from invisible (old) to more visible (new)
    float alpha = v_alpha * 0.5;  // Scale alpha (0.0 to 0.5)
    frag_color = vec4(0.6, 0.6, 0.6, alpha);
}
