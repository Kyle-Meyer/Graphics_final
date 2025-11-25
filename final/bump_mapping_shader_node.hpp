#ifndef __FINAL_BUMP_MAPPING_SHADER_NODE_HPP__
#define __FINAL_BUMP_MAPPING_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"
#include "scene/image_data.hpp"
#include "scene/color4.hpp"

namespace cg
{

/**
 * Bump mapping shader node that extends standard lighting shader.
 * Provides normal/bump mapping support with full Phong lighting.
 */
class BumpMappingShaderNode : public ShaderNode
{
  public:
    BumpMappingShaderNode();
    virtual ~BumpMappingShaderNode();

    /**
     * Get uniform and attribute locations from the shader program.
     * @return True if all required locations were found.
     */
    bool get_locations() override;

    /**
     * Draw method - enables shader, sets uniforms, draws children.
     * @param scene_state Current scene state.
     */
    void draw(SceneState &scene_state) override;

    /**
     * Bind a normal map texture.
     * @param im_data Image data for the normal map.
     * @return True if successful.
     */
    bool bind_normal_map(ImageData *im_data);

    /**
     * Set bump/normal map strength.
     * @param strength Strength factor (1.0 = normal, 0.0 = flat, >1.0 = exaggerated)
     */
    void set_bump_strength(float strength);

    /**
     * Enable or disable normal mapping.
     * @param enabled True to enable normal mapping.
     */
    void set_normal_mapping_enabled(bool enabled);

    /**
     * Set global ambient light color.
     * @param ambient Ambient light color.
     */
    void set_global_ambient(const Color4 &ambient);

    // Attribute location getters for geometry creation
    int32_t get_position_loc() const { return position_loc_; }
    int32_t get_normal_loc() const { return normal_loc_; }
    int32_t get_texcoord_loc() const { return texcoord_loc_; }
    int32_t get_tangent_loc() const { return tangent_loc_; }
    int32_t get_bitangent_loc() const { return bitangent_loc_; }

  protected:
    // Attribute locations (match Module10 pattern - use glGetAttribLocation)
    GLint position_loc_;
    GLint normal_loc_;
    GLint texcoord_loc_;
    GLint tangent_loc_;
    GLint bitangent_loc_;

    // Matrix uniform locations
    GLint pvm_matrix_loc_;
    GLint model_matrix_loc_;
    GLint normal_matrix_loc_;
    GLint camera_position_loc;  // No trailing underscore, matches LightingShaderNode

    // Material uniform locations
    GLint material_ambient_loc_;
    GLint material_diffuse_loc_;
    GLint material_specular_loc_;
    GLint material_emission_loc_;
    GLint material_shininess_loc_;

    // Lighting uniforms (match LightingShaderNode pattern EXACTLY)
    int32_t       light_count_;
    GLint         light_count_loc_;
    GLint         global_ambient_loc_;
    LightUniforms lights_[3];  // Support 3 lights like LightingShaderNode

    // Normal map uniform locations
    GLint normal_map_loc_;
    GLint use_normal_map_loc_;
    GLint bump_strength_loc_;

    // Normal map state
    GLuint normal_map_texture_id_;
    bool   normal_map_bound_;
    bool   normal_mapping_enabled_;
    float  bump_strength_;
};

} // namespace cg

#endif
