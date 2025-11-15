#include "final/multi_texture_shader_node.hpp"

#include <iostream>

namespace cg
{

MultiTextureShaderNode::MultiTextureShaderNode()
    : blend_mode_(BlendMode::MIX), mix_factor_(0.5f)
{
    node_type_ = SceneNodeType::SHADER;
    
    // Initialize texture state
    for(int i = 0; i < MAX_TEXTURES; i++)
    {
        texture_ids_[i] = 0;
        texture_bound_[i] = false;
        texture_enabled_[i] = false;
    }
    
    init_textures();
}

MultiTextureShaderNode::~MultiTextureShaderNode()
{
    cleanup_textures();
}

void MultiTextureShaderNode::init_textures()
{
    // Generate texture objects
    glGenTextures(MAX_TEXTURES, texture_ids_);
}

void MultiTextureShaderNode::cleanup_textures()
{
    // Delete texture objects
    glDeleteTextures(MAX_TEXTURES, texture_ids_);
}

bool MultiTextureShaderNode::get_locations()
{
    // Get attribute locations
    position_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_position");
    if(position_loc_ < 0)
    {
        std::cout << "Error getting vtx_position location\n";
        return false;
    }

    normal_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_normal");
    if(normal_loc_ < 0)
    {
        std::cout << "Error getting vtx_normal location\n";
        return false;
    }

    texcoord_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_texcoord");
    if(texcoord_loc_ < 0)
    {
        std::cout << "Error getting vtx_texcoord location\n";
        return false;
    }

    // Get matrix uniform locations
    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    if(pvm_matrix_loc_ < 0)
    {
        std::cout << "Error getting pvm_matrix location\n";
        return false;
    }

    model_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "model_matrix");
    if(model_matrix_loc_ < 0)
    {
        std::cout << "Warning: model_matrix location not found (may be optimized out)\n";
    }

    normal_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "normal_matrix");
    if(normal_matrix_loc_ < 0)
    {
        std::cout << "Warning: normal_matrix location not found (may be optimized out)\n";
    }

    // Get multi-texture uniform locations
    char name[64];
    for(int i = 0; i < MAX_TEXTURES; i++)
    {
        snprintf(name, 64, "texture_sampler%d", i);
        texture_sampler_locs_[i] = glGetUniformLocation(shader_program_.get_program(), name);
        if(texture_sampler_locs_[i] < 0)
        {
            std::cout << "Warning: " << name << " location not found\n";
        }

        snprintf(name, 64, "texture_enabled%d", i);
        texture_enabled_locs_[i] = glGetUniformLocation(shader_program_.get_program(), name);
        if(texture_enabled_locs_[i] < 0)
        {
            std::cout << "Warning: " << name << " location not found\n";
        }
    }

    blend_mode_loc_ = glGetUniformLocation(shader_program_.get_program(), "blend_mode");
    if(blend_mode_loc_ < 0)
    {
        std::cout << "Warning: blend_mode location not found\n";
    }

    mix_factor_loc_ = glGetUniformLocation(shader_program_.get_program(), "mix_factor");
    if(mix_factor_loc_ < 0)
    {
        std::cout << "Warning: mix_factor location not found\n";
    }

    return true;
}

void MultiTextureShaderNode::draw(SceneState &scene_state)
{
    // Enable this program
    shader_program_.use();

    // Set scene state locations
    scene_state.position_loc = position_loc_;
    scene_state.normal_loc = normal_loc_;
    scene_state.texcoord_loc = texcoord_loc_;
    scene_state.pvm_matrix_loc = pvm_matrix_loc_;
    scene_state.model_matrix_loc = model_matrix_loc_;
    scene_state.normal_matrix_loc = normal_matrix_loc_;

    // Bind and activate all enabled textures
    for(int i = 0; i < MAX_TEXTURES; i++)
    {
        if(texture_bound_[i])
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture_ids_[i]);
            glUniform1i(texture_sampler_locs_[i], i);
        }
        glUniform1i(texture_enabled_locs_[i], texture_enabled_[i] ? 1 : 0);
    }

    // Set blend mode uniform
    glUniform1i(blend_mode_loc_, static_cast<int>(blend_mode_));
    
    // Set mix factor uniform
    glUniform1f(mix_factor_loc_, mix_factor_);

    // Draw all children
    SceneNode::draw(scene_state);
}

bool MultiTextureShaderNode::bind_texture(int unit, ImageData *im_data)
{
    if(unit < 0 || unit >= MAX_TEXTURES)
    {
        std::cout << "Invalid texture unit: " << unit << "\n";
        return false;
    }

    if(im_data == nullptr || im_data->data == nullptr)
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

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    texture_bound_[unit] = true;
    texture_enabled_[unit] = true;

    std::cout << "Texture bound to unit " << unit << " (" << im_data->w << "x" 
              << im_data->h << ", " << im_data->channels << " channels)\n";

    return true;
}

void MultiTextureShaderNode::set_blend_mode(BlendMode mode)
{
    blend_mode_ = mode;
}

void MultiTextureShaderNode::set_mix_factor(float factor)
{
    // Clamp factor between 0.0 and 1.0
    if(factor < 0.0f)
        mix_factor_ = 0.0f;
    else if(factor > 1.0f)
        mix_factor_ = 1.0f;
    else
        mix_factor_ = factor;
}

void MultiTextureShaderNode::set_texture_enabled(int unit, bool enabled)
{
    if(unit >= 0 && unit < MAX_TEXTURES)
    {
        texture_enabled_[unit] = enabled;
    }
}

} // namespace cg
