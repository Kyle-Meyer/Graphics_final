//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    scene_node.hpp
//	Purpose: Scene graph node. All scene node classes inherit from this
//           base class.
//
//============================================================================

#ifndef __SCENE_SCENE_NODE_HPP__
#define __SCENE_SCENE_NODE_HPP__

#include "scene/graphics.hpp"
#include "scene/scene_state.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace cg
{

enum class SceneNodeType
{
    BASE,
    PRESENTATION,
    TRANSFORM,
    GEOMETRY,
    SHADER,
    CAMERA,
    LIGHT
};

std::ostream &operator<<(std::ostream &out, const SceneNodeType &type);

/**
 * Scene graph node: base class
 */
class SceneNode
{
  public:
    /**
     * Constructor. Set the reference count to 0.
     */
    SceneNode();

    /**
     * Destructor
     */
    virtual ~SceneNode();

    /**
     * Draw the scene node and its children. The base class just draws the
     * children. Derived classes can use this (SceneNode::draw()) to draw
     * all children without having to duplicate this code.
     * @param  scene_state  Current scene state
     */
    virtual void draw(SceneState &scene_state);

    /**
     * Update the scene node and its children
     * @param  scene_state  Current scene state
     */
    virtual void update(SceneState &scene_state);

    /**
     * Destroy all the children
     */
    void destroy();

    /**
     * Add a child to this node. Increment the reference count of the child.
     * @param  node  Add a child node to this scene node.
     */
    void add_child(std::shared_ptr<SceneNode> node);

    /**
     * Get the type of scene node
     * @return  Returns the type of hte scene node.
     */
    SceneNodeType node_type() const;

    /**
     * Set the name for this scene node.
     * @param  nm  Name for this scene node.
     */
    void set_name(const char *nm);

    /**
     * Get the name for this scene node.
     * @return  Returns the name of this scene node.
     */
    const std::string &get_name() const;

    void print_graph(std::ostream &out = std::cout, int32_t level = 0) const;

  protected:
    std::string                             name_;
    SceneNodeType                           node_type_;
    std::vector<std::shared_ptr<SceneNode>> children_;
};

} // namespace cg

#endif
