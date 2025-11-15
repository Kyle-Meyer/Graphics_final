#include "Module10/lighting_shader_node.hpp"

#include <cstdio>
#include <iostream>

namespace cg
{

bool LightingShaderNode::get_locations()
{
    position_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_position");
    if(position_loc_ < 0)
    {
        std::cout << "Error getting vtx_position location\n";
        return false;
    }
    vertex_normal_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_normal");
    if(vertex_normal_loc_ < 0)
    {
        std::cout << "Error getting vtx_normal location\n";
        return false;
    }

    texcoord_loc_ = glGetAttribLocation(shader_program_.get_program(), "vtx_texcoord");
    if(texcoord_loc_ < 0)
    {
        std::cout << "Warning: vtx_texcoord location not found (may be optimized out if not used)\n";
    }

    pvm_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "pvm_matrix");
    if(pvm_matrix_loc_ < 0)
    {
        std::cout << "Error getting pvm_matrix location\n";
        return false;
    }
    model_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "model_matrix");
    if(model_matrix_loc_ < 0)
    {
        std::cout << "Error getting model_matrix location\n";
        return false;
    }

    normal_matrix_loc_ = glGetUniformLocation(shader_program_.get_program(), "normal_matrix");
    if(normal_matrix_loc_ < 0)
    {
        std::cout << "Error getting normal_matrix location\n";
        return false;
    }

    // Populate camera position uniform location in scene state
    camera_position_loc = glGetUniformLocation(shader_program_.get_program(), "camera_position");

    // Set the number of lights to 3
    light_count_ = 3;
    light_count_loc_ = glGetUniformLocation(shader_program_.get_program(), "num_lights");
    if(light_count_loc_ < 0)
    {
        std::cout << "LightingShaderNode: Error getting num_lights location\n";
    }

    // Get light uniforms
    char name[128];
    for(int i = 0; i < light_count_; i++)
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
    global_ambient_loc_ =
        glGetUniformLocation(shader_program_.get_program(), "global_light_ambient");
    if(global_ambient_loc_ < 0)
    {
        std::cout << "LightingShaderNode: Error getting global ambient location\n";
    }

    // Populate material uniform locations in scene state
    material_ambient_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_ambient");
    material_diffuse_loc_ = glGetUniformLocation(shader_program_.get_program(), "material_diffuse");
    material_specular_loc_ =
        glGetUniformLocation(shader_program_.get_program(), "material_specular");
    material_emission_loc_ =
        glGetUniformLocation(shader_program_.get_program(), "material_emission");
    material_shininess_loc_ =
        glGetUniformLocation(shader_program_.get_program(), "material_shininess");

    texture_sampler_loc_ = glGetUniformLocation(shader_program_.get_program(), "texture_sampler");
    if(texture_sampler_loc_ < 0)
    {
        std::cout << "Warning: texture_sampler location not found\n";
    }

    use_texture_loc_ = glGetUniformLocation(shader_program_.get_program(), "use_texture");
    if(use_texture_loc_ < 0)
    {
        std::cout << "Warning: use_texture location not found\n";
    }

    return true;
}

void LightingShaderNode::draw(SceneState &scene_state)
{
    // Enable this program
    shader_program_.use();

    // Set scene state locations to ones needed for this program
    scene_state.position_loc = position_loc_;
    scene_state.normal_loc = vertex_normal_loc_;
    scene_state.texcoord_loc = texcoord_loc_; 
    scene_state.pvm_matrix_loc = pvm_matrix_loc_;
    scene_state.model_matrix_loc = model_matrix_loc_;
    scene_state.normal_matrix_loc = normal_matrix_loc_;
    scene_state.camera_position_loc = camera_position_loc;

    // Set material uniform location
    scene_state.material_ambient_loc = material_ambient_loc_;
    scene_state.material_diffuse_loc = material_diffuse_loc_;
    scene_state.material_specular_loc = material_specular_loc_;
    scene_state.material_emission_loc = material_emission_loc_;
    scene_state.material_shininess_loc = material_shininess_loc_;

    scene_state.texture_sampler_loc = texture_sampler_loc_;
    scene_state.use_texture_loc = use_texture_loc_;

    // Set the light locations
    scene_state.lightcount_loc = light_count_loc_;
    for(uint32_t i = 0; i < light_count_; i++) { scene_state.lights[i] = lights_[i]; }

    // Draw all children
    SceneNode::draw(scene_state);
}

void LightingShaderNode::set_global_ambient(const Color4 &global_ambient)
{
    shader_program_.use();
    glUniform4fv(global_ambient_loc_, 1, &global_ambient.r);
}

int LightingShaderNode::get_position_loc() const { return position_loc_; }

int LightingShaderNode::get_normal_loc() const { return vertex_normal_loc_; }

int LightingShaderNode::get_texcoord_loc() const { return texcoord_loc_; }

} // namespace cg
