
//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	David W. Nesbitt
//	File:    color_node.hpp
//	Purpose:	Simple presentation node that defines a color.
//
//============================================================================

#ifndef __SCENE_COLOR_NODE_HPP__
#define __SCENE_COLOR_NODE_HPP__

#include "scene/color4.hpp"
#include "scene/presentation_node.hpp"

namespace cg
{

/**
 * Color presentation node.
 */
class ColorNode : public PresentationNode
{
  public:
    /**
     * Constructor.
     */
    ColorNode();

    /**
     * Constructor given a red, green, and blue value.
     */
    ColorNode(const Color4 &c);

    /**
     * Draw this presentation node and its children
     */
    void draw(SceneState &scene_state) override;

  protected:
    Color4 material_color_;
};

} // namespace cg

#endif
