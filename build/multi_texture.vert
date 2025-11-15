#version 410 core

// Multi-texturing vertex shader
// Passes through position, normal, and texture coordinates

// Incoming vertex attributes
layout (location = 0) in vec3 vtx_position;
layout (location = 1) in vec3 vtx_normal;
layout (location = 2) in vec2 vtx_texcoord;

// Outgoing interpolated values
layout (location = 0) smooth out vec3 normal;
layout (location = 1) smooth out vec3 vertex;
layout (location = 2) smooth out vec2 texcoord;

// Uniforms for matrices
uniform mat4 pvm_matrix;    // Composite projection, view, model matrix
uniform mat4 model_matrix;  // Modeling matrix (for world space position)
uniform mat4 normal_matrix; // Normal transformation matrix

void main()
{
    // Transform normal to world space using normal matrix
    normal = normalize(vec3(normal_matrix * vec4(vtx_normal, 0.0)));
    
    // Transform vertex position to world space using model matrix
    // This is needed for proper lighting calculations in fragment shader
    vertex = vec3(model_matrix * vec4(vtx_position, 1.0));

    // Pass through texture coordinates
    texcoord = vtx_texcoord;

    // Convert position to clip coordinates using PVM matrix
    gl_Position = pvm_matrix * vec4(vtx_position, 1.0);
}
