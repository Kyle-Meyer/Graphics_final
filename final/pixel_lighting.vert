#version 410 core

// Incoming vertex and normal attributes
// Vertex position attribute
layout (location = 0) in vec3 vtx_position;
// Vertex normal attribute
layout (location = 1) in vec3 vtx_normal;

layout (location = 2) in vec2 vtx_texcoord;

// Outgoing normal and vertex (interpolated) in world coordinates
layout (location = 0) smooth out vec3 normal;
layout (location = 1) smooth out vec3 vertex;

layout (location = 2) smooth out vec2 texcoord;

// Uniforms for matrices
uniform mat4 pvm_matrix;	// Composite projection, view, model matrix
uniform mat4 model_matrix;	// Modeling  matrix
uniform mat4 normal_matrix;	// Normal transformation matrix

// Simple shader for Phong (per-pixel) shading. The fragment shader will
// do all the work. We need to pass per-vertex normals to the fragment
// shader. We also will transform the vertex into world coordinates so 
// the fragment shader can interpolate world coordinates.
void main()
{
	// Transform normal and position to world coords. 
	normal = normalize(vec3(normal_matrix * vec4(vtx_normal, 0.0)));
	vertex = vec3((model_matrix * vec4(vtx_position, 1.0)));

	// Convert position to clip coordinates and pass along
	gl_Position = pvm_matrix * vec4(vtx_position, 1.0);
   texcoord = vtx_texcoord;
}
