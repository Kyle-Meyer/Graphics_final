#include "final/bump_multi_texture_shader_node.hpp"

#include <cstdio>
#include <iostream>

namespace cg
{

BumpMultiTextureShaderNode::BumpMultiTextureShaderNode()
    : normal_map_texture_id_(0),
      normal_map_bound_(false),
      normal_mapping_enabled_(true),
      bump_strength_(1.0f),
      light_count_(3),
      blend_mode_(BlendMode::MIX),
      mix_factor_(0.5f),
      cloud_rotation_(0.0f)
{
    node_type_ = SceneNodeType::SHADER;

    // Initialize color texture state
    for (int i = 0; i < MAX_COLOR_TEXTURES; i++)
    {
        texture_ids_[i] = 0;
        texture_bound_[i] = false;
        texture_enabled_[i] = false;
    }

    init_textures();
}

BumpMultiTextureShaderNode::~BumpMultiTextureShaderNode()
{
    cleanup_textures();
}

void BumpMultiTextureShaderNode::init_textures()
{
    // Generate normal map texture
    glGenTextures(1, &normal_map_texture_id_);

    // Generate color texture objects
    glGenTextures(MAX_COLOR_TEXTURES, texture_ids_);
}

void BumpMultiTextureShaderNode::cleanup_textures()
{
    // Delete normal map texture
    if (normal_map_texture_id_ != 0)
    {
        glDeleteTextures(1, &normal_map_texture_id_);
    }

    // Delete color texture objects
    glDeleteTextures(MAX_COLOR_TEXTURES, texture_ids_);
}

bool BumpMultiTextureShaderNode::get_locations()
{
    // Get attribute locations using glGetAttribLocation
    position_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_position");
    if (position_loc_ < 0)
    {
        std::cout << "Error getting vtx_position location\n";
        return false;
    }

    normal_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_normal");
    if (normal_loc_ < 0)
    {
        std::cout << "Error getting vtx_normal location\n";
        return false;
    }

    texcoord_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_texcoord");
    if (texcoord_loc_ < 0)
    {
        std::cout << "Warning: vtx_texcoord location not found\n";
    }

    tangent_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_tangent");
    if (tangent_loc_ < 0)
    {
        std::cout << "Warning: vtx_tangent location not found\n";
    }

    bitangent_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_bitangent");
    if (bitangent_loc_ < 0)
    {
        std::cout << "Warning: vtx_bitangent location not found\n";
    }

    // Get matrix uniform locations
    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    if (pvm_matrix_loc_ < 0)
    {
        std::cout << "Error getting pvm_matrix location\n";
        return false;
    }

    model_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "model_matrix");
    if (model_matrix_loc_ < 0)
    {
        std::cout << "Error getting model_matrix location\n";
        return false;
    }

    normal_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "normal_matrix");
    if (normal_matrix_loc_ < 0)
    {
        std::cout << "Error getting normal_matrix location\n";
        return false;
    }

    camera_position_loc = glGetUniformLocation(shader_program_.get_program(), "camera_position");
    if (camera_position_loc < 0)
    {
        std::cout << "Error getting camera_position location\n";
        return false;
    }

    // Get material uniform locations
    material_ambient_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_ambient");
    material_diffuse_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_diffuse");
    material_specular_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_specular");
    material_emission_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_emission");
    material_shininess_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_shininess");

    // Get lighting uniform locations
    light_count_loc_ = glGetUniformLocation(shader_program_.get_program(), "num_lights");
    if (light_count_loc_ < 0)
    {
        std::cout << "Error getting num_lights location\n";
        return false;
    }

    global_ambient_loc_ = glGetUniformLocation(shader_program_.get_program(), "global_light_ambient");
    if (global_ambient_loc_ < 0)
    {
        std::cout << "Error getting global_light_ambient location\n";
        return false;
    }

    // Get light array uniforms
    char name[128];
    for (int i = 0; i < light_count_; i++)
    {
        snprintf(name, 128, "lights[%d].enabled", i);
        lights_[i].enabled = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].spotlight", i);
        lights_[i].spotlight = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].position", i);
        lights_[i].position = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].ambient", i);
        lights_[i].ambient = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].diffuse", i);
        lights_[i].diffuse = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].specular", i);
        lights_[i].specular = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].constant_attenuation", i);
        lights_[i].att_constant = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].linear_attenuation", i);
        lights_[i].att_linear = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].quadratic_attenuation", i);
        lights_[i].att_quadratic = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].spot_cutoff", i);
        lights_[i].spot_cutoff = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].spot_exponent", i);
        lights_[i].spot_exponent = glGetUniformLocation(shader_program_.get_program(), name);

        snprintf(name, 128, "lights[%d].spot_direction", i);
        lights_[i].spot_direction = glGetUniformLocation(shader_program_.get_program(), name);
    }

    // Get normal map uniform locations
    normal_map_loc_ = glGetUniformLocation(shader_program_.get_program(), "normal_map");
    use_normal_map_loc_ = glGetUniformLocation(shader_program_.get_program(), "use_normal_map");
    bump_strength_loc_ = glGetUniformLocation(shader_program_.get_program(), "bump_strength");

    // Get multi-texture uniform locations
    for (int i = 0; i < MAX_COLOR_TEXTURES; i++)
    {
        snprintf(name, 128, "texture_sampler%d", i);
        texture_sampler_locs_[i] = glGetUniformLocation(shader_program_.get_program(), name);
        if (texture_sampler_locs_[i] < 0)
        {
            std::cout << "Warning: " << name << " location not found\n";
        }

        snprintf(name, 128, "texture_enabled%d", i);
        texture_enabled_locs_[i] = glGetUniformLocation(shader_program_.get_program(), name);
        if (texture_enabled_locs_[i] < 0)
        {
            std::cout << "Warning: " << name << " location not found\n";
        }
    }

    blend_mode_loc_ = glGetUniformLocation(shader_program_.get_program(), "blend_mode");
    if (blend_mode_loc_ < 0)
    {
        std::cout << "Warning: blend_mode location not found\n";
    }

    mix_factor_loc_ = glGetUniformLocation(shader_program_.get_program(), "mix_factor");
    if (mix_factor_loc_ < 0)
    {
        std::cout << "Warning: mix_factor location not found\n";
    }

    cloud_rotation_loc_ = glGetUniformLocation(shader_program_.get_program(), "cloud_rotation");
    if (cloud_rotation_loc_ < 0)
    {
        std::cout << "Warning: cloud_rotation location not found\n";
    }

    std::cout << "BumpMultiTextureShaderNode: All shader locations retrieved\n";
    return true;
}

void BumpMultiTextureShaderNode::draw(SceneState &scene_state)
{
    // Enable this shader program
    shader_program_.use();

    // Set scene state locations
    scene_state.position_loc = position_loc_;
    scene_state.normal_loc = normal_loc_;
    scene_state.texcoord_loc = texcoord_loc_;
    scene_state.tangent_loc = tangent_loc_;
    scene_state.bitangent_loc = bitangent_loc_;

    scene_state.pvm_matrix_loc = pvm_matrix_loc_;
    scene_state.model_matrix_loc = model_matrix_loc_;
    scene_state.normal_matrix_loc = normal_matrix_loc_;
    scene_state.camera_position_loc = camera_position_loc;

    // Set material uniform locations
    scene_state.material_ambient_loc = material_ambient_loc_;
    scene_state.material_diffuse_loc = material_diffuse_loc_;
    scene_state.material_specular_loc = material_specular_loc_;
    scene_state.material_emission_loc = material_emission_loc_;
    scene_state.material_shininess_loc = material_shininess_loc_;

    // Set global ambient uniform
    glUniform4f(global_ambient_loc_, 0.1f, 0.1f, 0.1f, 1.0f);

    // Set number of lights uniform
    glUniform1i(light_count_loc_, 1);

    // Set camera position
    glUniform3f(camera_position_loc,
                scene_state.camera_position.x,
                scene_state.camera_position.y,
                scene_state.camera_position.z);

    // Set light state
    glUniform1i(lights_[0].enabled, 1);
    glUniform1i(lights_[0].spotlight, 0);
    glUniform4f(lights_[0].position, 0.0f, -50.0f, 80.0f, 1.0f);
    glUniform4f(lights_[0].ambient, 0.1f, 0.1f, 0.1f, 1.0f);  // Reduced ambient
    glUniform4f(lights_[0].diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(lights_[0].specular, 0.8f, 0.8f, 0.8f, 1.0f);  // Reduced specular
    glUniform1f(lights_[0].att_constant, 1.0f);
    glUniform1f(lights_[0].att_linear, 0.0f);
    glUniform1f(lights_[0].att_quadratic, 0.0f);

    // Bind and activate all enabled color textures
    for (int i = 0; i < MAX_COLOR_TEXTURES; i++)
    {
        if (texture_bound_[i])
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture_ids_[i]);
            glUniform1i(texture_sampler_locs_[i], i);
        }
        glUniform1i(texture_enabled_locs_[i], texture_enabled_[i] ? 1 : 0);
    }

    // Set blend mode and mix factor uniforms
    glUniform1i(blend_mode_loc_, static_cast<int>(blend_mode_));
    glUniform1f(mix_factor_loc_, mix_factor_);

    // Update and set cloud rotation (rotate at 60 degrees per second at 60fps)
    // Negative value = opposite direction of Earth rotation
    cloud_rotation_ -= 0.01745f;  // 60 degrees/sec * (PI/180) / 60fps = 0.01745 radians/frame
    if (cloud_rotation_ <= -6.28319f) {  // -2*PI
        cloud_rotation_ += 6.28319f;
    }
    glUniform1f(cloud_rotation_loc_, cloud_rotation_);

    // Bind normal map if available
    if (normal_map_bound_)
    {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, normal_map_texture_id_);
        glUniform1i(normal_map_loc_, 5);
    }

    // Set normal mapping uniforms
    glUniform1i(use_normal_map_loc_, (normal_map_bound_ && normal_mapping_enabled_) ? 1 : 0);
    glUniform1f(bump_strength_loc_, bump_strength_);

    // Draw all children
    SceneNode::draw(scene_state);
}

// ========== BUMP MAPPING METHODS ==========

bool BumpMultiTextureShaderNode::bind_normal_map(ImageData *im_data)
{
    if (im_data == nullptr || im_data->data == nullptr)
    {
        std::cout << "Error: Invalid image data for normal map\n";
        return false;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normal_map_texture_id_);

    GLenum format = (im_data->channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, im_data->w, im_data->h,
                 0, format, GL_UNSIGNED_BYTE, im_data->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    normal_map_bound_ = true;

    std::cout << "Normal map bound: " << im_data->w << "x" << im_data->h
              << " (" << im_data->channels << " channels)\n";

    return true;
}

void BumpMultiTextureShaderNode::set_bump_strength(float strength)
{
    bump_strength_ = strength;
}

void BumpMultiTextureShaderNode::set_normal_mapping_enabled(bool enabled)
{
    normal_mapping_enabled_ = enabled;
}

void BumpMultiTextureShaderNode::set_global_ambient(const Color4 &ambient)
{
    shader_program_.use();
    glUniform4fv(global_ambient_loc_, 1, &ambient.r);
}

// ========== MULTI-TEXTURE METHODS ==========

bool BumpMultiTextureShaderNode::bind_texture(int unit, ImageData *im_data)
{
    if (unit < 0 || unit >= MAX_COLOR_TEXTURES)
    {
        std::cout << "Invalid texture unit: " << unit << "\n";
        return false;
    }

    if (im_data == nullptr || im_data->data == nullptr)
    {
        std::cout << "Invalid image data for texture unit " << unit << "\n";
        return false;
    }

    // Activate the texture unit
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_ids_[unit]);

    // Determine format based on channels
    GLenum format = (im_data->channels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, im_data->w, im_data->h,
                 0, format, GL_UNSIGNED_BYTE, im_data->data);

    // Set texture parameters based on unit
    if (unit == 1) {
        // Cloud texture (unit 1) - clamp to avoid repeating artifacts at seams
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Use linear filtering for smooth cartoon edges
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        // Other textures - use repeat and mipmaps for better quality
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    texture_bound_[unit] = true;
    texture_enabled_[unit] = true;

    std::cout << "Color texture bound to unit " << unit << " (" << im_data->w << "x"
              << im_data->h << ", " << im_data->channels << " channels)\n";

    return true;
}

void BumpMultiTextureShaderNode::set_blend_mode(BlendMode mode)
{
    blend_mode_ = mode;
}

void BumpMultiTextureShaderNode::set_mix_factor(float factor)
{
    // Clamp factor between 0.0 and 1.0
    if (factor < 0.0f)
        mix_factor_ = 0.0f;
    else if (factor > 1.0f)
        mix_factor_ = 1.0f;
    else
        mix_factor_ = factor;
}

void BumpMultiTextureShaderNode::set_texture_enabled(int unit, bool enabled)
{
    if (unit >= 0 && unit < MAX_COLOR_TEXTURES)
    {
        texture_enabled_[unit] = enabled;
    }
}

} // namespace cg
