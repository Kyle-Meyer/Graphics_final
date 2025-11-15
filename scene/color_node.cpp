#include "scene/color_node.hpp"

namespace cg
{

ColorNode::ColorNode() : PresentationNode() {}

ColorNode::ColorNode(const Color4 &c) { material_color_ = c; }

void ColorNode::draw(SceneState &scene_state)
{
    // Set the current color and draw all children. Very simple lighting support
    glUniform3fv(scene_state.material_diffuse_loc, 1, &material_color_.r);
    SceneNode::draw(scene_state);
}

} // namespace cg
