#include "final/bump_mapping_shader_node.hpp"

#include <cstdio>
#include <iostream>

namespace cg
{

BumpMappingShaderNode::BumpMappingShaderNode()
    : normal_map_texture_id_(0),
      normal_map_bound_(false),
      normal_mapping_enabled_(true),
      bump_strength_(1.0f)
{
    node_type_ = SceneNodeType::SHADER;

    // Generate texture for normal map
    glGenTextures(1, &normal_map_texture_id_);
}

BumpMappingShaderNode::~BumpMappingShaderNode()
{
    if (normal_map_texture_id_ != 0)
    {
        glDeleteTextures(1, &normal_map_texture_id_);
    }
}

bool BumpMappingShaderNode::get_locations()
{
    GLuint prog = shader_program_.get_program();

    // Matrix uniforms
    pvm_matrix_loc_ = glGetUniformLocation(prog, "pvm_matrix");
    if (pvm_matrix_loc_ < 0)
    {
        std::cout << "Error: pvm_matrix uniform not found\n";
        return false;
    }

    model_matrix_loc_ = glGetUniformLocation(prog, "model_matrix");
    if (model_matrix_loc_ < 0)
    {
        std::cout << "Warning: model_matrix uniform not found (may be optimized out)\n";
    }

    normal_matrix_loc_ = glGetUniformLocation(prog, "normal_matrix");
    if (normal_matrix_loc_ < 0)
    {
        std::cout << "Warning: normal_matrix uniform not found (may be optimized out)\n";
    }

    // Material uniforms
    material_ambient_loc_ = glGetUniformLocation(prog, "material_ambient");
    material_diffuse_loc_ = glGetUniformLocation(prog, "material_diffuse");
    material_specular_loc_ = glGetUniformLocation(prog, "material_specular");
    material_emission_loc_ = glGetUniformLocation(prog, "material_emission");
    material_shininess_loc_ = glGetUniformLocation(prog, "material_shininess");

    // Lighting uniforms
    global_ambient_loc_ = glGetUniformLocation(prog, "global_ambient");
    camera_position_loc_ = glGetUniformLocation(prog, "camera_position");
    num_lights_loc_ = glGetUniformLocation(prog, "num_lights");

    // Light array uniforms
    char name[128];
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        snprintf(name, 128, "lights[%d].enabled", i);
        light_locs_[i].enabled = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].is_spotlight", i);
        light_locs_[i].is_spotlight = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].position", i);
        light_locs_[i].position = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].ambient", i);
        light_locs_[i].ambient = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].diffuse", i);
        light_locs_[i].diffuse = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].specular", i);
        light_locs_[i].specular = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].constant_atten", i);
        light_locs_[i].constant_atten = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].linear_atten", i);
        light_locs_[i].linear_atten = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].quadratic_atten", i);
        light_locs_[i].quadratic_atten = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].spot_cutoff", i);
        light_locs_[i].spot_cutoff = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].spot_exponent", i);
        light_locs_[i].spot_exponent = glGetUniformLocation(prog, name);

        snprintf(name, 128, "lights[%d].spot_direction", i);
        light_locs_[i].spot_direction = glGetUniformLocation(prog, name);
    }

    // Normal map uniforms
    normal_map_loc_ = glGetUniformLocation(prog, "normal_map");
    use_normal_map_loc_ = glGetUniformLocation(prog, "use_normal_map");
    bump_strength_loc_ = glGetUniformLocation(prog, "bump_strength");

    std::cout << "BumpMappingShaderNode: All shader locations retrieved\n";
    return true;
}

void BumpMappingShaderNode::draw(SceneState &scene_state)
{
    // Enable this shader program
    shader_program_.use();

    // Set attribute locations in scene state for geometry to use
    scene_state.position_loc = position_loc_;
    scene_state.normal_loc = normal_loc_;
    scene_state.texcoord_loc = texcoord_loc_;
    scene_state.tangent_loc = tangent_loc_;
    scene_state.bitangent_loc = bitangent_loc_;

    // Set matrix uniform locations in scene state
    scene_state.pvm_matrix_loc = pvm_matrix_loc_;
    scene_state.model_matrix_loc = model_matrix_loc_;
    scene_state.normal_matrix_loc = normal_matrix_loc_;

    // Set material uniform locations in scene state
    scene_state.material_ambient_loc = material_ambient_loc_;
    scene_state.material_diffuse_loc = material_diffuse_loc_;
    scene_state.material_specular_loc = material_specular_loc_;
    scene_state.material_emission_loc = material_emission_loc_;
    scene_state.material_shininess_loc = material_shininess_loc_;

    // Set light uniform locations in scene state
    scene_state.camera_position_loc = camera_position_loc_;
    scene_state.lightcount_loc = num_lights_loc_;
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        scene_state.lights[i].enabled = light_locs_[i].enabled;
        scene_state.lights[i].spotlight = light_locs_[i].is_spotlight;
        scene_state.lights[i].position = light_locs_[i].position;
        scene_state.lights[i].ambient = light_locs_[i].ambient;
        scene_state.lights[i].diffuse = light_locs_[i].diffuse;
        scene_state.lights[i].specular = light_locs_[i].specular;
        scene_state.lights[i].att_constant = light_locs_[i].constant_atten;
        scene_state.lights[i].att_linear = light_locs_[i].linear_atten;
        scene_state.lights[i].att_quadratic = light_locs_[i].quadratic_atten;
        scene_state.lights[i].spot_cutoff = light_locs_[i].spot_cutoff;
        scene_state.lights[i].spot_exponent = light_locs_[i].spot_exponent;
        scene_state.lights[i].spot_direction = light_locs_[i].spot_direction;
    }

    // Set camera position uniform
    glUniform3f(camera_position_loc_,
                scene_state.camera_position.x,
                scene_state.camera_position.y,
                scene_state.camera_position.z);

    // Bind normal map if available
    if (normal_map_bound_)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normal_map_texture_id_);
        glUniform1i(normal_map_loc_, 0);
    }

    // Set normal mapping uniforms
    glUniform1i(use_normal_map_loc_, (normal_map_bound_ && normal_mapping_enabled_) ? 1 : 0);
    glUniform1f(bump_strength_loc_, bump_strength_);

    // Draw children (lights, transforms, geometry, etc.)
    SceneNode::draw(scene_state);
}

bool BumpMappingShaderNode::bind_normal_map(ImageData *im_data)
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

void BumpMappingShaderNode::set_bump_strength(float strength)
{
    bump_strength_ = strength;
}

void BumpMappingShaderNode::set_normal_mapping_enabled(bool enabled)
{
    normal_mapping_enabled_ = enabled;
}

void BumpMappingShaderNode::set_global_ambient(const Color4 &ambient)
{
    shader_program_.use();
    glUniform4fv(global_ambient_loc_, 1, &ambient.r);
}

} // namespace cg
