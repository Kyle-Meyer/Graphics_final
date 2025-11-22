#ifndef __FINAL_BUMP_MAPPING_SHADER_NODE_HPP__
#define __FINAL_BUMP_MAPPING_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"
#include "scene/image_data.hpp"
#include "scene/color4.hpp"

namespace cg
{

/**
 * Bump mapping shader node. Provides normal/bump mapping support
 * with full Phong lighting. Independent of multi-texturing.
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
    // Attribute locations (explicit layout in shader)
    static constexpr GLint position_loc_ = 0;
    static constexpr GLint normal_loc_ = 1;
    static constexpr GLint texcoord_loc_ = 2;
    static constexpr GLint tangent_loc_ = 3;
    static constexpr GLint bitangent_loc_ = 4;

    // Matrix uniform locations
    GLint pvm_matrix_loc_;
    GLint model_matrix_loc_;
    GLint normal_matrix_loc_;

    // Material uniform locations
    GLint material_ambient_loc_;
    GLint material_diffuse_loc_;
    GLint material_specular_loc_;
    GLint material_emission_loc_;
    GLint material_shininess_loc_;

    // Lighting uniform locations
    GLint global_ambient_loc_;
    GLint camera_position_loc_;
    GLint num_lights_loc_;

    // Light array uniform locations
    struct LightLocs {
        GLint enabled;
        GLint is_spotlight;
        GLint position;
        GLint ambient;
        GLint diffuse;
        GLint specular;
        GLint constant_atten;
        GLint linear_atten;
        GLint quadratic_atten;
        GLint spot_cutoff;
        GLint spot_exponent;
        GLint spot_direction;
    };
    static constexpr int MAX_LIGHTS = 8;
    LightLocs light_locs_[MAX_LIGHTS];

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
