#version 410 core

// Multi-texturing fragment shader
// Supports multiple textures with various blend modes

// Incoming interpolated values
layout (location = 0) smooth in vec3 normal;
layout (location = 1) smooth in vec3 vertex;
layout (location = 2) smooth in vec2 texcoord;

// Output fragment color
layout (location = 0) out vec4 frag_color;

// Texture samplers (support up to 4 textures)
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

// Material color (used when no textures are enabled)
uniform vec3 material_diffuse;

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

void main()
{
    // Start with white color
    vec4 final_color = vec4(1.0);
    int active_textures = 0;
    
    // Sample enabled textures
    vec4 tex_colors[4];
    int enabled_count = 0;
    
    if (texture_enabled0 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler0, texcoord);
        enabled_count++;
    }
    
    if (texture_enabled1 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler1, texcoord);
        enabled_count++;
    }
    
    if (texture_enabled2 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler2, texcoord);
        enabled_count++;
    }
    
    if (texture_enabled3 == 1)
    {
        tex_colors[enabled_count] = texture(texture_sampler3, texcoord);
        enabled_count++;
    }
    
    // Blend textures together
    if (enabled_count == 0)
    {
        // No textures enabled, use material color
        final_color = vec4(material_diffuse, 1.0);
    }
    else if (enabled_count == 1)
    {
        // Single texture
        final_color = tex_colors[0];
    }
    else if (enabled_count == 2)
    {
        // Two textures - simple blend
        final_color = blend_colors(tex_colors[0], tex_colors[1], blend_mode, mix_factor);
    }
    else if (enabled_count == 3)
    {
        // Three textures - blend first two, then blend result with third
        vec4 blended = blend_colors(tex_colors[0], tex_colors[1], blend_mode, mix_factor);
        final_color = blend_colors(blended, tex_colors[2], blend_mode, mix_factor);
    }
    else if (enabled_count == 4)
    {
        // Four textures - progressive blending
        vec4 blended1 = blend_colors(tex_colors[0], tex_colors[1], blend_mode, mix_factor);
        vec4 blended2 = blend_colors(tex_colors[2], tex_colors[3], blend_mode, mix_factor);
        final_color = blend_colors(blended1, blended2, blend_mode, 0.5);
    }
    
    // Add basic shading based on normal
    vec3 light_dir = normalize(vec3(0.5, 0.5, 1.0));
    vec3 n = normalize(normal);
    float diffuse = max(dot(n, light_dir), 0.0);
    float ambient = 0.3;
    float lighting = ambient + (1.0 - ambient) * diffuse;
    
    // Apply lighting
    final_color.rgb *= lighting;
    
    // Clamp and output
    frag_color = clamp(final_color, 0.0, 1.0);
}
