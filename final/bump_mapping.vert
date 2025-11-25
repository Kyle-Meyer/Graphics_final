#version 410 core

// Vertex attributes with explicit layout locations
layout(location = 0) in vec3 vtx_position;
layout(location = 1) in vec3 vtx_normal;
layout(location = 2) in vec2 vtx_texcoord;
layout(location = 3) in vec3 vtx_tangent;
layout(location = 4) in vec3 vtx_bitangent;

// Outputs to fragment shader
out vec3 frag_position;    // World space position
out vec3 frag_normal;      // World space normal
out vec2 frag_texcoord;    // Texture coordinates
out vec3 frag_tangent;     // World space tangent
out vec3 frag_bitangent;   // World space bitangent

// Transformation matrices
uniform mat4 pvm_matrix;      // Projection * View * Model
uniform mat4 model_matrix;    // Model matrix
uniform mat4 normal_matrix;   // Transpose of inverse model matrix

void main()
{
    // Transform position to clip space
    gl_Position = pvm_matrix * vec4(vtx_position, 1.0);

    // Transform position to world space for lighting
    frag_position = vec3(model_matrix * vec4(vtx_position, 1.0));

    // Transform TBN basis vectors to world space
    frag_normal = normalize(vec3(normal_matrix * vec4(vtx_normal, 0.0)));
    frag_tangent = normalize(vec3(normal_matrix * vec4(vtx_tangent, 0.0)));
    frag_bitangent = normalize(vec3(normal_matrix * vec4(vtx_bitangent, 0.0)));

    // Pass texture coordinates through
    frag_texcoord = vtx_texcoord;
}


