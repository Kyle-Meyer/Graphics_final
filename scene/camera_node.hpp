//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    camera_node.hpp
//	Purpose: Provide support for setting the view and updating the view.
//
//============================================================================

#ifndef __SCENE_CAMERA_NODE_HPP__
#define __SCENE_CAMERA_NODE_HPP__

#include "scene/scene_node.hpp"

namespace cg
{

enum class ProjectionType
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

/**
 * Camera node class.
 */
class CameraNode : public SceneNode
{
  public:
    /**
     * Constructor.
     */
    CameraNode();

    /**
     * Draw the scene node and its children. The base class just draws the
     * children. Derived classes can use this (SceneNode::draw()) to draw
     * all children without having to duplicate this code.
     */
    void draw(SceneState &scene_state) override;

    /**
     * Sets the view reference point (camera position)
     *	@param	vrp		View reference point.
     */
    void set_position(const Point3 &vrp);

    /**
     * Gets the view reference point (camera position)
     * @return		Returns the view reference point.
     */
    const Point3 &get_position() const;

    /**
     * Sets the lookat point.
     * @param	lp		Lookat point.
     */
    void set_look_at_pt(const Point3 &lp);

    /**
     * Gets the lookat point.
     * @return	Returns the lookat point.
     */
    const Point3 &get_look_at_pt() const;

    /**
     * Sets the view up direction.
     * @param	vup		View up direction.
     */
    void set_view_up(const Vector3 &vup);

    /**
     * Gets the view plane normal (vz axis).
     * @return	Returns the view plane normal (vz)
     */
    const Vector3 &get_view_plane_normal() const;

    /**
     *	Gets the view right axis (vx axis).
     * @return	Returns the view right axis (vx)
     */
    const Vector3 &get_view_right() const;

    /**
     * Gets the view up axis (vy axis).
     * @return	Returns the view up axis (vy)
     */
    const Vector3 &get_view_up() const;

    /**
     * Roll the camera by the specified degrees about the VPN.
     * Rotation about n. Note that the lookat point does not change
     * @param  degrees   Rotation angle (degrees)
     */
    void roll(float degrees);

    /**
     * Change the pitch of the camera by the specified degrees.
     * This is a rotation about u
     * @param  degrees   Rotation angle (degrees)
     */
    void pitch(float degrees);

    /**
     * Change the heading of the camera by the specified degrees.
     * This is a rotation about v
     * @param  degrees   Rotation angle (degrees)
     */
    void heading(float degrees);

    /**
     * Move and Turn.  Move the LookAt point by the specified vector.
     * The VRP is then moved so that it is the same distance from the
     * LookAt point as previous.  This effectively moves and turns.
     *
     * NOTE: a different method would involve a rotation using pitch and
     * heading and then move forward or backwards along the new VPN. I figure
     * that method is more common and wanted to present the method below
     * as an alternate way to approach the problem
     */
    void move_and_turn(float right, float up, float forward);

    /**
     * Slide the camera in the specified lengths along the view axes.
     * @param  x   Amount to move along u
     * @param  y   Amount to move along v
     * @param  z   Amount to move along n
     */
    void slide(float x, float y, float z);

    /**
     * Gets the current matrix (used to store modeling transforms).
     *	@return	Returns the current modeling/viewing composite matrix.
     */
    const Matrix4x4 &get_view_matrix() const;

    /**
     * Sets a symmetric perspective projection
     * @param  fov  Field of view angle y (degrees)
     * @param  ratio Aspect ratio (width / height)
     * @param  n   Near plane distance (must be positive)
     * @param  f   Far plane distance (must be positive)
     */
    void set_perspective(float fov, float ratio, float n, float f);

    /**
     * Change the field of view.
     * @param  fov  Field of view angle y (degrees)
     */
    void change_field_of_view(float fov);

    /**
     * Change the aspect ratio.
     * @param  ratio  Aspect ratio (width / height)
     */
    void change_aspect_ratio(float ratio);

    /**
     * Change the near_clip and far_clip clipping planes.
     * @param  n  Near plane distance (must be positive)
     * @param  f  Far plane distance (must be positive)
     */
    void change_clipping_planes(float n, float f);

  protected:
    // Perspective projection parameters
    float fov_;          // Field of view in degrees
    float aspect_ratio_; // Aspect ratio (width / height)
    float near_clip_;    // Near clipping plane distance
    float far_clip_;     // Far clipping plane distance

    Point3 vrp_; // View point (eye)
    Point3 lpt_; // Lookat point

    // View axes
    Vector3 view_normal_; // View plane normal
    Vector3 view_right_;  // View right axis
    Vector3 view_up_;     // View up axis

    // Matrices
    Matrix4x4 view_; // Viewing matrix
    Matrix4x4 proj_; // Projection matrix

    // Sets the view axes
    void look_at();

    // Sets the persective projection matrix
    void set_perspective();

    // Create viewing transformation matrix by composing the translation
    // matrix with the rotation matrix given by the view coordinate axes
    void set_view_matrix();
};

} // namespace cg

#endif
