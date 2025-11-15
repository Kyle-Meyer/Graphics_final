//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    transform_node.hpp
//	Purpose: Scene graph transformation node.
//
//============================================================================

#ifndef __SCENE_TRANSFORM_NODE_HPP__
#define __SCENE_TRANSFORM_NODE_HPP__

#include "scene/scene_node.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

/**
 * Transform node. Applies a transformation. This class allows OpenGL style
 * transforms applied to the scene graph.
 */
class TransformNode : public SceneNode
{
  public:
    /**
     * Constructor.
     */
    TransformNode();

    /**
     * Destructor.
     */
    virtual ~TransformNode();

    /**
     * Set the identity matrix
     */
    void load_identity();

    /**
     * Apply a translation
     * @param  x  x translation
     * @param  y  y translation
     * @param  z  z translation
     */
    void translate(float x, float y, float z);

    /**
     * Apply a general axis rotation.
     * @param  deg  Degrees counter-clockwise rotation.
     * @param  v    Rotation axis
     */
    void rotate(float deg, Vector3 &v);

    /**
     * Apply rotation about the x axis.
     * @param  deg  Degrees counter-clockwise rotation.
     */
    void rotate_x(float deg);

    /**
     * Apply rotation about the y axis.
     * @param  deg  Degrees counter-clockwise rotation.
     */
    void rotate_y(float deg);

    /**
     * Apply rotation about the z axis.
     * @param  deg  Degrees counter-clockwise rotation.
     */
    void rotate_z(float deg);

    /**
     * Apply a scaling.
     * @param  x  x scaling factor
     * @param  y  y scaling factor
     * @param  z  z scaling factor
     */
    void scale(float x, float y, float z);

    /**
     * Draw this transformation node and its children
     * @param  scene_state   Current scene state
     */
    void draw(SceneState &scene_state) override;

    /**
     * Update the scene node and its children
     * @param  scene_state   Current scene state
     */
    void update(SceneState &scene_state) override;

  protected:
    Matrix4x4 model_matrix_; // Local modeling transformation
};

} // namespace cg

#endif
