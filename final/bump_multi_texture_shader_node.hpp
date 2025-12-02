#ifndef __FINAL_BUMP_MULTI_TEXTURE_SHADER_NODE_HPP__
#define __FINAL_BUMP_MULTI_TEXTURE_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"
#include "scene/image_data.hpp"
#include "scene/color4.hpp"
#include "final/multi_texture_shader_node.hpp"  // For BlendMode enum

namespace cg
{

/**
 * Combined bump mapping and multi-texture shader node.
 * Provides normal/bump mapping support with multi-texture blending and full Phong lighting.
 */
class BumpMultiTextureShaderNode : public ShaderNode
{
  public:
    BumpMultiTextureShaderNode();
    virtual ~BumpMultiTextureShaderNode();

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

    // ========== BUMP MAPPING METHODS ==========

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

    // ========== MULTI-TEXTURE METHODS ==========

    /**
     * Bind a color texture to a specific texture unit (0-3).
     * @param unit Texture unit index (0-3)
     * @param im_data Pointer to image data to bind
     * @return Returns true if successful
     */
    bool bind_texture(int unit, ImageData *im_data);

    /**
     * Set the blend mode for multi-texturing.
     * @param mode Blend mode to use
     */
    void set_blend_mode(BlendMode mode);

    /**
     * Set the mix factor for blending (used in MIX mode).
     * @param factor Mix factor between 0.0 (first texture) and 1.0 (second texture)
     */
    void set_mix_factor(float factor);

    /**
     * Enable/disable a specific texture unit.
     * @param unit Texture unit index
     * @param enabled True to enable, false to disable
     */
    void set_texture_enabled(int unit, bool enabled);

    /**
     * Get the current blend mode.
     * @return Current blend mode
     */
    BlendMode get_blend_mode() const { return blend_mode_; }

    // ========== ATTRIBUTE LOCATION GETTERS ==========

    int32_t get_position_loc() const { return position_loc_; }
    int32_t get_normal_loc() const { return normal_loc_; }
    int32_t get_texcoord_loc() const { return texcoord_loc_; }
    int32_t get_tangent_loc() const { return tangent_loc_; }
    int32_t get_bitangent_loc() const { return bitangent_loc_; }

  protected:
    static constexpr int MAX_COLOR_TEXTURES = 4;

    // Attribute locations
    GLint position_loc_;
    GLint normal_loc_;
    GLint texcoord_loc_;
    GLint tangent_loc_;
    GLint bitangent_loc_;

    // Matrix uniform locations
    GLint pvm_matrix_loc_;
    GLint model_matrix_loc_;
    GLint normal_matrix_loc_;
    GLint camera_position_loc;

    // Material uniform locations
    GLint material_ambient_loc_;
    GLint material_diffuse_loc_;
    GLint material_specular_loc_;
    GLint material_emission_loc_;
    GLint material_shininess_loc_;

    // Lighting uniforms
    int32_t       light_count_;
    GLint         light_count_loc_;
    GLint         global_ambient_loc_;
    LightUniforms lights_[3];  // Support 3 lights

    // Normal map state and uniforms
    GLuint normal_map_texture_id_;
    bool   normal_map_bound_;
    bool   normal_mapping_enabled_;
    float  bump_strength_;
    GLint  normal_map_loc_;
    GLint  use_normal_map_loc_;
    GLint  bump_strength_loc_;

    // Color texture state and uniforms
    GLuint texture_ids_[MAX_COLOR_TEXTURES];
    bool   texture_bound_[MAX_COLOR_TEXTURES];
    bool   texture_enabled_[MAX_COLOR_TEXTURES];
    GLint  texture_sampler_locs_[MAX_COLOR_TEXTURES];
    GLint  texture_enabled_locs_[MAX_COLOR_TEXTURES];

    // Blend mode state and uniforms
    BlendMode blend_mode_;
    float     mix_factor_;
    GLint     blend_mode_loc_;
    GLint     mix_factor_loc_;

    // Cloud animation
    float     cloud_rotation_;
    GLint     cloud_rotation_loc_;

    /**
     * Initialize texture objects.
     */
    void init_textures();

    /**
     * Clean up texture resources.
     */
    void cleanup_textures();
};

} // namespace cg

#endif
