//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    presentation_node.hpp
//	Purpose: Scene graph presentation node.
//
//============================================================================

#ifndef __SCENE_PRESENTATION_NODE_HPP__
#define __SCENE_PRESENTATION_NODE_HPP__

#include "scene/color4.hpp"
#include "scene/scene_node.hpp"

#include <string>

namespace cg
{

/**
 * Presentation node. Holds material properties and texture.
 */
class PresentationNode : public SceneNode
{
  public:
    /**
     * Constructor
     */
    PresentationNode();

    /**
     * Constructor given material properties.
     * @param  ambient  Material ambient reflection coefficients (color).
     * @param  diffuse  Material diffuse reflection coefficients (color).
     * @param  specular  Material specular reflection coefficients (color).
     * @param  emission  Material emission (color).
     * @param  shininess   Material shininess.
     */
    PresentationNode(const Color4 &ambient,
                     const Color4 &diffuse,
                     const Color4 &specular,
                     const Color4 &emission,
                     float         shininess);

    /**
     * Destructor
     */
    ~PresentationNode();

    /**
     * Set material ambient reflection coefficient.
     * @param  c  Ambient reflection coefficients (color).
     */
    void set_material_ambient(const Color4 &c);

    /**
     * Set material diffuse reflection coefficient.
     * @param  c  Diffuse reflection coefficients (color).
     */
    void set_material_diffuse(const Color4 &c);

    /**
     * Set material ambient and diffuse reflection coefficient to the
     * same value.
     * @param  c  Ambient/diffuse reflection coefficients (color).
     */
    void set_material_ambient_and_diffuse(const Color4 &c);

    /**
     * Set material specular reflection coefficient.
     * @param  c  Specular reflection coefficients (color).
     */
    void set_material_specular(const Color4 &c);

    /**
     * Set material emission.
     * @param  c  Emission (color).
     */
    void set_material_emission(const Color4 &c);

    /**
     * Set material shininess.
     * @param  s  Shininess.
     */
    void set_material_shininess(float s);

    /**
     * NEW: Load a texture from file.
     * @param  filename     Texture file name (will search in textures/ directory)
     * @param  use_mipmaps  Whether to generate and use mipmaps (default true)
     * @return  Returns true if successful
     */
    bool load_texture(const std::string &filename, bool use_mipmaps = true);

    /**
     * NEW: Enable or disable texture usage.
     * @param  enable  True to enable texture, false to disable
     */
    void enable_texture(bool enable);

    /**
     * NEW: Bind the texture for rendering.
     */
    void bind_texture();

    /**
     * Draw. Sets the material properties and texture.
     * @param  scene_state  Scene state (holds material uniform locations)
     */
    void draw(SceneState &scene_state) override;

  protected:
    Color4  material_ambient_;
    Color4  material_diffuse_;
    Color4  material_specular_;
    Color4  material_emission_;
    GLfloat material_shininess_;

    // NEW: Texture properties
    GLuint texture_id_;
    bool   has_texture_;
    bool   use_texture_;
};

} // namespace cg

#endif
