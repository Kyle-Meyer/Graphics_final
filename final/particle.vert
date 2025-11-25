#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 pvm_matrix;
uniform float point_size;

void main()
{
    gl_Position = pvm_matrix * vec4(position, 1.0);
    gl_PointSize = point_size;
}
