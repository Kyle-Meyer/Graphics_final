//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    geometry_node.hpp
//	Purpose: Scene graph geometry node.
//
//============================================================================

#ifndef __SCENE_GEOMETRY_NODE_HPP__
#define __SCENE_GEOMETRY_NODE_HPP__

#include "scene/scene_node.hpp"

namespace cg
{

/**
 * Geometry node base class. Stores and draws geometry.
 */
class GeometryNode : public SceneNode
{
  public:
    /**
     * Constructor
     */
    GeometryNode();

    /**
     * Destructor
     */
    virtual ~GeometryNode();

    /**
     * Draw this geometry node. Geometry nodes are leaf nodes and have no children.
     * @param  scene_state  Current scene state
     */
    virtual void draw(SceneState &scene_state) override;
};

} // namespace cg

#endif
