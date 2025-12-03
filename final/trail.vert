#version 410 core

// Trail shader with per-vertex alpha for fading effect

layout(location = 0) in vec3 position;
layout(location = 1) in float alpha;  // Alpha value (0 = oldest, 1 = newest)

uniform mat4 pvm_matrix;

out float v_alpha;

void main()
{
    gl_Position = pvm_matrix * vec4(position, 1.0);
    v_alpha = alpha;
}
