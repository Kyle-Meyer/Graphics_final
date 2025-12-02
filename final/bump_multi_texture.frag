#version 410 core

// Inputs from vertex shader
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texcoord;
in vec3 frag_tangent;
in vec3 frag_bitangent;

// Output color
out vec4 out_color;

// Material properties
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;

// Lighting
uniform vec4 global_light_ambient;
uniform vec3 camera_position;

// Light structure (support up to 8 lights)
const int MAX_LIGHTS = 8;
struct Light {
    int  enabled;
    int  spotlight;
    vec4 position;      // w=0 directional, w=1 point
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
uniform Light lights[MAX_LIGHTS];
uniform int num_lights;

// Normal map texture
uniform sampler2D normal_map;
uniform bool use_normal_map;
uniform float bump_strength;

// Color texture samplers (support up to 4 textures)
uniform sampler2D texture_sampler0;
uniform sampler2D texture_sampler1;
uniform sampler2D texture_sampler2;
uniform sampler2D texture_sampler3;

// Texture enable flags
uniform int texture_enabled0;
uniform int texture_enabled1;
uniform int texture_enabled2;
uniform int texture_enabled3;

// Blend mode: 0=MIX, 1=MULTIPLY, 2=ADD, 3=SUBTRACT
uniform int blend_mode;

// Mix factor for linear interpolation (used in MIX mode)
uniform float mix_factor;

// Cloud rotation angle (in radians) for animating cloud layer
uniform float cloud_rotation;

// Blend two colors based on the current blend mode
vec4 blend_colors(vec4 color1, vec4 color2, int mode, float factor)
{
    if (mode == 0) // MIX
    {
        return mix(color1, color2, factor);
    }
    else if (mode == 1) // MULTIPLY
    {
        return color1 * color2;
    }
    else if (mode == 2) // ADD
    {
        return color1 + color2;
    }
    else if (mode == 3) // SUBTRACT
    {
        return color1 - color2;
    }

    // Default to mix
    return mix(color1, color2, factor);
}

// Compute attenuation for point/spot lights
float compute_attenuation(int i, float dist)
{
    return 1.0 / (lights[i].constant_attenuation +
                  lights[i].linear_attenuation * dist +
                  lights[i].quadratic_attenuation * dist * dist);
}

void main()
{
    // ========== MULTI-TEXTURE BLENDING ==========
    // Sample and blend all enabled color textures
    vec4 tex_colors[4];
    int enabled_count = 0;

    if (texture_enabled0 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler0, frag_texcoord);
        enabled_count++;
    }

    if (texture_enabled1 == 1)
    {
        // Scroll cloud texture horizontally (around equator)
        // Offset the U coordinate to move clouds horizontally
        vec2 cloud_coord = frag_texcoord;
        cloud_coord.x = fract(frag_texcoord.x + cloud_rotation / 6.28319);  // Normalize rotation to 0-1 range

        tex_colors[enabled_count] = texture(texture_sampler1, cloud_coord);
        enabled_count++;
    }

    if (texture_enabled2 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler2, frag_texcoord);
        enabled_count++;
    }

    if (texture_enabled3 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler3, frag_texcoord);
        enabled_count++;
    }

    // Blend textures together to get final texture color
    vec4 blended_texture = vec4(1.0);

    if (enabled_count == 0)
    {
        // No textures enabled, use material diffuse
        blended_texture = material_diffuse;
    }
    else if (enabled_count == 1)
    {
        // Single texture
        blended_texture = tex_colors[0];
    }
    else if (enabled_count == 2)
    {
        // Two textures - simple blend
        blended_texture = blend_colors(tex_colors[0], tex_colors[1], blend_mode, mix_factor);
    }
    else if (enabled_count == 3)
    {
        // Three textures - blend first two, then blend result with third
        vec4 blended = blend_colors(tex_colors[0], tex_colors[1], blend_mode, mix_factor);
        blended_texture = blend_colors(blended, tex_colors[2], blend_mode, mix_factor);
    }
    else if (enabled_count == 4)
    {
        // Four textures - progressive blending
        vec4 blended1 = blend_colors(tex_colors[0], tex_colors[1], blend_mode, mix_factor);
        vec4 blended2 = blend_colors(tex_colors[2], tex_colors[3], blend_mode, mix_factor);
        blended_texture = blend_colors(blended1, blended2, blend_mode, 0.5);
    }

    // ========== NORMAL/BUMP MAPPING ==========
    // Get the interpolated normal (will be perturbed if normal mapping enabled)
    vec3 N = normalize(frag_normal);

    // Apply normal mapping if enabled
    if (use_normal_map)
    {
        // Compute normals from height map using gradient
        // Sample height at current position and neighbors
        vec2 texel_size = vec2(1.0 / 225.0);  // Texture size

        float h_center = texture(normal_map, frag_texcoord).r;
        float h_right = texture(normal_map, frag_texcoord + vec2(texel_size.x, 0.0)).r;
        float h_up = texture(normal_map, frag_texcoord + vec2(0.0, texel_size.y)).r;

        // Compute gradients (slope in U and V directions)
        float dU = (h_right - h_center) * bump_strength;
        float dV = (h_up - h_center) * bump_strength;

        // Construct perturbed normal in tangent space
        // Tangent space: X=tangent (U dir), Y=bitangent (V dir), Z=normal
        vec3 map_normal = normalize(vec3(-dU, -dV, 1.0));

        // Construct TBN matrix (tangent space to world space)
        vec3 T = normalize(frag_tangent);
        vec3 B = normalize(frag_bitangent);
        mat3 TBN = mat3(T, B, N);

        // Transform normal from tangent space to world space
        N = normalize(TBN * map_normal);
    }

    // ========== PHONG LIGHTING ==========
    // View direction (from fragment to camera)
    vec3 V = normalize(camera_position - frag_position);

    // Accumulate lighting contributions
    vec4 ambient_total = vec4(0.0);
    vec4 diffuse_total = vec4(0.0);
    vec4 specular_total = vec4(0.0);

    for (int i = 0; i < num_lights && i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 0) continue;

        vec3 L;
        float attenuation = 1.0;

        if (lights[i].position.w == 0.0)
        {
            // Directional light - position is direction
            L = normalize(lights[i].position.xyz);
        }
        else
        {
            // Point or spot light
            vec3 light_vec = lights[i].position.xyz - frag_position;
            float dist = length(light_vec);
            L = light_vec / dist;
            attenuation = compute_attenuation(i, dist);

            // Spotlight cone
            if (lights[i].spotlight == 1)
            {
                float spot_cos = dot(-L, normalize(lights[i].spot_direction));
                if (spot_cos < lights[i].spot_cutoff)
                {
                    attenuation = 0.0;
                }
                else
                {
                    attenuation *= pow(spot_cos, lights[i].spot_exponent);
                }
            }
        }

        // Ambient contribution
        ambient_total += lights[i].ambient * attenuation;

        // Diffuse contribution (Lambertian)
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            diffuse_total += lights[i].diffuse * NdotL * attenuation;

            // Specular contribution (Blinn-Phong)
            vec3 H = normalize(L + V);
            float NdotH = max(dot(N, H), 0.0);
            if (NdotH > 0.0)
            {
                specular_total += lights[i].specular * pow(NdotH, material_shininess) * attenuation;
            }
        }
    }

    // Combine material properties with lighting
    // Use blended_texture as the diffuse color
    vec4 color = material_emission
               + global_light_ambient * material_ambient
               + ambient_total * material_ambient
               + diffuse_total * blended_texture  // Use blended texture color instead of material_diffuse
               + specular_total * material_specular;

    out_color = clamp(color, 0.0, 1.0);
}
