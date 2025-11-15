#include "scene/scene_node.hpp"

namespace cg
{

std::ostream &operator<<(std::ostream &out, const SceneNodeType &type)
{
    switch(type)
    {
        case SceneNodeType::BASE: out << "SceneNodeType::BASE"; break;
        case SceneNodeType::PRESENTATION: out << "SceneNodeType::PRESENTATION"; break;
        case SceneNodeType::TRANSFORM: out << "SceneNodeType::TRANSFORM"; break;
        case SceneNodeType::GEOMETRY: out << "SceneNodeType::GEOMETRY"; break;
        case SceneNodeType::SHADER: out << "SceneNodeType::SHADER"; break;
        case SceneNodeType::CAMERA: out << "SceneNodeType::CAMERA"; break;
        case SceneNodeType::LIGHT: out << "SceneNodeType::LIGHT"; break;
        default: out << "[UNKNOWN TYPE]"; break;
    }
    return out;
}

SceneNode::SceneNode() : node_type_(SceneNodeType::BASE) {}

SceneNode::~SceneNode() { destroy(); }

void SceneNode::draw(SceneState &scene_state)
{
    // Loop through the list and draw the children
    for(auto c : children_) { c->draw(scene_state); }
}

void SceneNode::update(SceneState &scene_state)
{
    // Loop through the list and update the children
    for(auto c : children_) { c->update(scene_state); }
}

void SceneNode::destroy() { children_.clear(); }

void SceneNode::add_child(std::shared_ptr<SceneNode> node) { children_.push_back(node); }

SceneNodeType SceneNode::node_type() const { return node_type_; }

void SceneNode::set_name(const char *nm) { name_ = nm; }

const std::string &SceneNode::get_name() const { return name_; }

void SceneNode::print_graph(std::ostream &out, int32_t level) const
{
    for(size_t i = 0; i < level; ++i) out << "- ";

    if(name_.length() == 0) out << "[";
    else out << name_ << " - [";

    out << node_type_ << "]\n";

    for(auto c : children_) { c->print_graph(out, level + 1); }
}

} // namespace cg
