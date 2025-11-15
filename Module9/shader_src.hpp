#ifndef __MODULE9_SHADER_SRC_HPP__
#define __MODULE9_SHADER_SRC_HPP__

const char *pixel_lighting_vert =
    R"(
#version 410 core

layout (location = 0) in vec3 vtx_position;
layout (location = 1) in vec3 vtx_normal;
layout (location = 0) smooth out vec3 normal;
layout (location = 1) smooth out vec3 vertex;

uniform mat4 pvm_matrix;	// Composite projection, view, model matrix
uniform mat4 model_matrix;	// Modeling  matrix
uniform mat4 normal_matrix;	// Normal transformation matrix

void main()
{
	normal = normalize(vec3(normal_matrix * vec4(vtx_normal, 0.0)));
	vertex = vec3((model_matrix * vec4(vtx_position, 1.0)));
	gl_Position = pvm_matrix * vec4(vtx_position, 1.0);
}
)";

const char *pixel_lighting_frag =
    R"(
#version 410 core

layout (location = 0) smooth in vec3 normal;
layout (location = 1) smooth in vec3 vertex;
layout (location = 0) out vec4 frag_color; 

uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;
uniform vec4  global_light_ambient;
uniform vec3  camera_position;

uniform int num_lights;
const int MAX_LIGHTS = 8; 
struct LightSource
{
   int  enabled;
   int  spotlight;
   vec4 position;
   vec4 ambient;
   vec4 diffuse;
   vec4 specular;
   float constant_attenuation;
   float linear_attenuation;
   float quadratic_attenuation;
   float spot_cutoff;
   float spot_exponent;
   vec3  spot_direction;
};
uniform LightSource lights[MAX_LIGHTS]; 

float calculate_attenuation(in int i, in float distance)
{
  return (1.0 / (lights[i].constant_attenuation +
                (lights[i].linear_attenuation    * distance) +
                (lights[i].quadratic_attenuation * distance * distance)));
}

void directional_light(in int i, in vec3 N, in vec3 vtx, in vec3 V, inout vec4 ambient, 
				      inout vec4 diffuse, inout vec4 specular)
{
  ambient += lights[i].ambient;
  vec3 L = lights[i].position.xyz;
  float N_dot_L = dot(N, L);
  if (N_dot_L > 0.0)
  {
    diffuse += lights[i].diffuse * N_dot_L;
    vec3 H = normalize(L + V);
    float N_dot_H = dot(N, H);
    if (N_dot_H > 0.0) specular += lights[i].specular * pow(N_dot_H, material_shininess);
  }
}

void point_light(in int i, in vec3 N, in vec3 vtx, in vec3 V, inout vec4 ambient,
			    inout vec4 diffuse, inout vec4 specular)
{
  vec3 tmp = lights[i].position.xyz - vtx;
  float dist = length(tmp);
  vec3 L = tmp * (1.0 / dist);

  float attenuation = calculate_attenuation(i, dist);  
  ambient += lights[i].ambient * attenuation;
  float N_dot_L = dot(N, L);
  if (N_dot_L > 0.0)
  {
    diffuse += lights[i].diffuse  * attenuation * N_dot_L;
    vec3 H = normalize(L + V);
    float N_dot_H = dot(N, H);
    if (N_dot_H > 0.0) specular += lights[i].specular * attenuation * pow(N_dot_H, material_shininess);
  }
}

void spot_light(in int i, in vec3 N, in vec3 vtx, in vec3 V, inout vec4 ambient, 
		       inout vec4 diffuse, inout vec4 specular)
{
  vec3 tmp = lights[i].position.xyz - vtx;
  float dist = length(tmp);
  vec3 L = tmp * (1.0 / dist);

  float attenuation = calculate_attenuation(i, dist);  
  float N_dot_L = dot(N, L);
  if (N_dot_L > 0.0)
  {
    float spot_effect = dot(lights[i].spot_direction, -L);
    if (spot_effect > lights[i].spot_cutoff)
    {
      attenuation *= pow(spot_effect, lights[i].spot_exponent);
      diffuse += lights[i].diffuse  * attenuation * N_dot_L;
      vec3 H = normalize(L + V);
      float N_dot_H = dot(N, H);
      if (N_dot_H > 0.0) specular += lights[i].specular * attenuation * pow(N_dot_H, material_shininess);
    }
    else attenuation = 0.0;
  }

  ambient += lights[i].ambient * attenuation;
}

// Main fragment shader. 
void main()
{
  vec3 n = normalize(normal);
  vec3 V = normalize(camera_position - vertex);
  vec4 ambient  = vec4(0.0);
  vec4 diffuse  = vec4(0.0);
  vec4 specular = vec4(0.0);
  for (int i = 0; i < num_lights; i++)
	{
    if (lights[i].enabled != 1) continue;

    if (lights[i].position.w == 0.0) directional_light(i, n, vertex, V, ambient, diffuse, specular);
    else if (lights[i].spotlight == 1) spot_light(i, n, vertex, V, ambient, diffuse, specular);
    else point_light(i, n, vertex, V, ambient, diffuse, specular);
  }

	vec4 color = material_emission + global_light_ambient * material_ambient +
			(ambient  * material_ambient) + (diffuse  * material_diffuse) + (specular * material_specular);
     
	frag_color = clamp(color, 0.0, 1.0);
}

)";

#endif
