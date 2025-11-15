#version 410 core

// Phong shading. Fragment shader.

// Incoming, interpolated normal and vertex position in world coordinates
layout (location = 0) smooth in vec3 normal;
layout (location = 1) smooth in vec3 vertex;

layout (location = 0) out vec4 frag_color; 

// Uniforms for material properties
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;

// Global lighting environment ambient intensity
uniform vec4  global_light_ambient;

// Camera position in world coordinates
uniform vec3  camera_position;

// Number of active lights
uniform int num_lights;

// Structure for a light source. Allow up to 8 lights.
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

// Convenience method to compute attenuation for the ith light source
// given a distance
float calculate_attenuation(in int i, in float distance)
{
   return (1.0 / (lights[i].constant_attenuation +
                  (lights[i].linear_attenuation    * distance) +
                  (lights[i].quadratic_attenuation * distance * distance)));
}

// Convenience method to compute the ambient, diffuse, and specular
// contribution of directional light source i
void directional_light(in int i, in vec3 N, in vec3 vtx, in vec3 V, inout vec4 ambient, 
				      inout vec4 diffuse, inout vec4 specular)
{
   // Add light source ambient
   ambient += lights[i].ambient;

   // Get the light direction in world coordinates. Directional lights have
   // constant L (does not vary based on vertex position)- assume we have
   // normalized the light vector in the application
   vec3 L = lights[i].position.xyz;

   // Dot product of normal and light direction
   float N_dot_L = dot(N, L);
   if (N_dot_L > 0.0)
   {
      // Add light source diffuse modulated by NDotL
      diffuse += lights[i].diffuse * N_dot_L;
      
      // Construct the halfway vector - note that we are assuming local viewpoint
      vec3 H = normalize(L + V);
      
      // Find dot product of N and H and add specular contribution due to this light source
      float N_dot_H = dot(N, H);
      if (N_dot_H > 0.0) specular += lights[i].specular * pow(N_dot_H, material_shininess);
   }
}

// Convenience method to compute the ambient, diffuse, and specular
// contribution of point light source i
void point_light(in int i, in vec3 N, in vec3 vtx, in vec3 V, inout vec4 ambient,
			    inout vec4 diffuse, inout vec4 specular)
{
   // Construct a vector from the vertex to the light source. Find the length for
   // use in attenuation. Normalize that vector (L) by dividing through by length
   vec3 tmp = lights[i].position.xyz - vtx;
   float dist = length(tmp);
   vec3 L = tmp * (1.0 / dist);

   // Compute attenuation
   float attenuation = calculate_attenuation(i, dist);  

   // Attenuate the light source ambient contribution
   ambient += lights[i].ambient * attenuation;

   // Determine dot product of normal with L. If < 0 the light is not 
   // incident on the front face of the surface.
   float N_dot_L = dot(N, L);
   if (N_dot_L > 0.0)
   {
      // Add diffuse contribution of this light source
      diffuse += lights[i].diffuse  * attenuation * N_dot_L;

      // Construct the halfway vector and add specular contribution (if N dot H > 0)
      vec3 H = normalize(L + V);
      float N_dot_H = dot(N, H);
      if (N_dot_H > 0.0) specular += lights[i].specular * attenuation * pow(N_dot_H, material_shininess);
   }
}

void spot_light(in int i, in vec3 N, in vec3 vtx, in vec3 V, inout vec4 ambient, 
		       inout vec4 diffuse, inout vec4 specular)
{
   // Construct a vector from the vertex to the light source. Find the length for
   // use in attenuation. Normalize that vector (L) by dividing through by length
   vec3 tmp = lights[i].position.xyz - vtx;
   float dist = length(tmp);
   vec3 L = tmp * (1.0 / dist);

   // Compute attenuation
   float attenuation = calculate_attenuation(i, dist);  
      
   // Determine dot product of normal with L. If < 0 the light is not 
   // incident on the front face of the surface.
   float N_dot_L = dot(N, L);
   if (N_dot_L > 0.0)
   {
      // Get the spotlight effect
      float spot_effect = dot(lights[i].spot_direction, -L);

      // See if within the cutoff angle
      if (spot_effect > lights[i].spot_cutoff)
      {
         attenuation *= pow(spot_effect, lights[i].spot_exponent);
            
         // Add diffuse contribution of this light source
         diffuse += lights[i].diffuse  * attenuation * N_dot_L;

         // Construct the halfway vector and add specular contribution (if N dot H > 0)
         vec3 H = normalize(L + V);
         float N_dot_H = dot(N, H);
         if (N_dot_H > 0.0) specular += lights[i].specular * attenuation * pow(N_dot_H, material_shininess);
      }
      else attenuation = 0.0;
   }
   
   // Attenuate the light source ambient contribution (note that this can
   // be modulated by the spotlight and if outside the spotlight cutoff
   // we will have no ambient contribution. (Unclear if this is correct!)
   ambient += lights[i].ambient * attenuation;
}

// Main fragment shader. 
void main()
{
   // Normalize the normal - as a linear interpolation of normals can result
   // in non unit-length vectors. Cannot directly modify the varying value, so
   // create a temporary variable here. 
   vec3 n = normalize(normal);

   // Construct a unit length vector from the vertex to the camera  
   vec3 V = normalize(camera_position - vertex);

   // Iterate through all lights to determine the illumination striking this pixel. 
   // Use the uniform variable numlights passed in by the application
   // to go use only lights up to the highest number specified.
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

   // Compute color. Emmission + global ambient contribution + light sources ambient, diffuse,
   // and specular contributions
   vec4 color = material_emission + global_light_ambient * material_ambient +
			(ambient  * material_ambient) + (diffuse  * material_diffuse) + (specular * material_specular);
     
	frag_color = clamp(color, 0.0, 1.0);
}
