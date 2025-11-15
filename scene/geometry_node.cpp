#include "scene/geometry_node.hpp"

namespace cg
{

GeometryNode::GeometryNode() { node_type_ = SceneNodeType::GEOMETRY; }

GeometryNode::~GeometryNode() {}

void GeometryNode::draw(SceneState &scene_state) {}

} // namespace cg
