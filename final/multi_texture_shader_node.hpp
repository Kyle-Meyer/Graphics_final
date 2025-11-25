#ifndef __FINAL_MULTI_TEXTURE_SHADER_NODE_HPP__
#define __FINAL_MULTI_TEXTURE_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"
#include "scene/image_data.hpp"

namespace cg
{

/**
 * Blend modes for multi-texturing
 */
enum class BlendMode
{
    MIX,        // Linear interpolation (mix)
    MULTIPLY,   // Multiplicative blending
    ADD,        // Additive blending
    SUBTRACT    // Subtractive blending
};

/**
 * Multi-texture shader node. Supports binding multiple textures
 * and blending them together using various blend modes.
 */
class MultiTextureShaderNode : public ShaderNode
{
  public:
    /**
     * Constructor.
     */
    MultiTextureShaderNode();

    /**
     * Destructor - cleans up texture resources.
     */
    virtual ~MultiTextureShaderNode();

    /**
     * Gets uniform and attribute locations.
     */
    bool get_locations() override;

    /**
     * Draw method - enables the program and sets up uniforms
     * @param  scene_state   Current scene state.
     */
    void draw(SceneState &scene_state) override;

    /**
     * Bind a texture to a specific texture unit (0-3).
     * @param  unit       Texture unit index (0-3)
     * @param  im_data    Pointer to image data to bind
     * @return Returns true if successful
     */
    bool bind_texture(int unit, ImageData *im_data);

    /**
     * Set the blend mode for multi-texturing.
     * @param  mode  Blend mode to use
     */
    void set_blend_mode(BlendMode mode);

    /**
     * Set the mix factor for blending (used in MIX mode).
     * @param  factor  Mix factor between 0.0 (first texture) and 1.0 (second texture)
     */
    void set_mix_factor(float factor);

    /**
     * Enable/disable a specific texture unit.
     * @param  unit     Texture unit index
     * @param  enabled  True to enable, false to disable
     */
    void set_texture_enabled(int unit, bool enabled);

    /**
     * Get the current blend mode.
     * @return Current blend mode
     */
    BlendMode get_blend_mode() const { return blend_mode_; }

    /**
     * Get the vertex position attribute location.
     * @return  Returns the vertex position attribute location.
     */
    int32_t get_position_loc() const { return position_loc_; }

    /**
     * Get the vertex normal attribute location.
     * @return  Returns the vertex normal attribute location.
     */
    int32_t get_normal_loc() const { return normal_loc_; }

    /**
     * Get the texture coordinate attribute location.
     * @return  Returns the texture coordinate attribute location.
     */
    int32_t get_texcoord_loc() const { return texcoord_loc_; }

  protected:
    static constexpr int MAX_TEXTURES = 4;

    // Texture objects and state
    GLuint texture_ids_[MAX_TEXTURES];
    bool   texture_bound_[MAX_TEXTURES];
    bool   texture_enabled_[MAX_TEXTURES];

    // Blend mode
    BlendMode blend_mode_;
    float     mix_factor_;

    // Uniform and attribute locations
    GLint position_loc_;
    GLint normal_loc_;
    GLint texcoord_loc_;
    GLint pvm_matrix_loc_;
    GLint model_matrix_loc_;
    GLint normal_matrix_loc_;

    // Multi-texture specific uniforms
    GLint texture_sampler_locs_[MAX_TEXTURES];
    GLint texture_enabled_locs_[MAX_TEXTURES];
    GLint blend_mode_loc_;
    GLint mix_factor_loc_;
    GLint material_diffuse_loc_;

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
