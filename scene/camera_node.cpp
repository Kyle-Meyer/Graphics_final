#include "scene/camera_node.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

CameraNode::CameraNode()
{
    node_type_ = SceneNodeType::CAMERA;

    // Perspective settings
    fov_ = 50.0f;
    aspect_ratio_ = 1.0f;
    near_clip_ = 1.0f;
    far_clip_ = 1000.0f;

    // Initial view settings
    lpt_ = Point3(0.0f, 0.0f, 0.0f);
    vrp_ = Point3(0.0f, 0.0f, 1.0f);
    view_up_ = Vector3(0.0f, 1.0f, 0.0f);
}

void CameraNode::draw(SceneState &scene_state)
{
    scene_state.camera_position = vrp_;

    // Copy the current composite projection and viewing matrix to the scene state
    scene_state.pv = proj_ * view_;

    // Set the shader PVM matrix - this will allow drawing children without a TransformNode
    glUniformMatrix4fv(scene_state.pvm_matrix_loc, 1, GL_FALSE, scene_state.pv.get());

    // Set the camera position
    glUniform3fv(scene_state.camera_position_loc, 1, &vrp_.x);

    // Draw children
    SceneNode::draw(scene_state);
}

void CameraNode::set_position(const Point3 &vrp)
{
    vrp_ = vrp;
    look_at();
}

const Point3 &CameraNode::get_position() const { return vrp_; }

void CameraNode::set_look_at_pt(const Point3 &lp)
{
    lpt_ = lp;
    look_at();
}

const Point3 &CameraNode::get_look_at_pt() const { return lpt_; }

void CameraNode::set_view_up(const Vector3 &vup)
{
    view_up_ = vup;
    look_at();
}

const Vector3 &CameraNode::get_view_plane_normal() const { return view_normal_; }

const Vector3 &CameraNode::get_view_right() const { return view_right_; }

const Vector3 &CameraNode::get_view_up() const { return view_up_; }

void CameraNode::roll(float degrees)
{
    // Method below is described in FS Hill and lecture notes. Just
    // change the two axes and reset the view matrix
    float   radians = degrees_to_radians(degrees);
    float   c = std::cos(radians);
    float   s = std::sin(radians);
    Vector3 tmp_right = view_right_;
    view_right_ = c * tmp_right + s * view_up_;
    view_up_ = -s * tmp_right + c * view_up_;

    set_view_matrix();
}

void CameraNode::pitch(float degrees)
{
    // Method below is described in FS Hill and lecture notes. Just
    // change the two axes and reset the view matrix
    float   radians = degrees_to_radians(degrees);
    float   c = std::cos(radians);
    float   s = std::sin(radians);
    Vector3 tmp_up = view_up_;
    view_up_ = c * tmp_up + s * view_normal_;
    view_normal_ = -s * tmp_up + c * view_normal_;
    set_view_matrix();

    // Reset the lookat (keep the same distance)
    Vector3 v1 = vrp_ - lpt_;
    float   d = v1.norm();
    lpt_ = vrp_ - view_normal_ * d;
}

void CameraNode::heading(float degrees)
{
    // Method below is described in FS Hill and lecture notes. Just
    // change the two axes and reset the view matrix
    float   radians = degrees_to_radians(degrees);
    float   c = std::cos(radians);
    float   s = std::sin(radians);
    Vector3 tmp_right = view_right_;
    view_right_ = c * tmp_right - s * view_normal_;
    view_normal_ = s * tmp_right + c * view_normal_;
    set_view_matrix();

    // Reset the lookat (keep the same distance)
    Vector3 v1 = vrp_ - lpt_;
    float   d = v1.norm();
    lpt_ = vrp_ - view_normal_ * d;
}

void CameraNode::move_and_turn(float right, float up, float forward)
{
    // Construct a vector from current lookat to VRP
    Vector3 v1 = vrp_ - lpt_;
    float   d = v1.norm();

    // Move the LookAt point by specified distances along the 3 view axes.
    Vector3 dir = view_right_ * right + view_up_ * up + view_normal_ * -forward;
    lpt_ = lpt_ + dir;

    // Construct vector from new LookAt to current VRP
    Vector3 v2 = vrp_ - lpt_;

    // Find the projection of v1 onto v2. Keep same distance as previous
    Vector3 v3 = v1.projection(v2);
    v3.normalize();
    v3 *= d;

    // New VRP is the new lookat point plus v3
    vrp_ = lpt_ + v3;
    look_at();
}

void CameraNode::slide(float x, float y, float z)
{
    Vector3 mv = view_right_ * x + view_up_ * y + view_normal_ * z;
    vrp_ = vrp_ + mv;
    lpt_ = lpt_ + mv;
    look_at();
}

const Matrix4x4 &CameraNode::get_view_matrix() const { return view_; }

void CameraNode::set_perspective(float fov, float ratio, float n, float f)
{
    fov_ = fov;
    aspect_ratio_ = ratio;
    near_clip_ = n;
    far_clip_ = f;
    set_perspective();
}

void CameraNode::change_field_of_view(float fov)
{
    fov_ = fov;
    set_perspective();
}

void CameraNode::change_aspect_ratio(float ratio)
{
    aspect_ratio_ = ratio;
    set_perspective();
}

void CameraNode::change_clipping_planes(float n, float f)
{
    near_clip_ = n;
    far_clip_ = f;
    set_perspective();
}

void CameraNode::look_at()
{
    // Set the VPN, which is the vector vp - vc
    view_normal_ = vrp_ - lpt_;
    view_normal_.normalize();

    // Get the view right axis from UP cross VPN
    view_right_ = view_up_.cross(view_normal_);
    view_right_.normalize();

    // Calculate the view up axis from VPN cross VRT.
    // (No need to normalize here since VPN and VRT are orthogonal
    // and unit length)
    view_up_ = view_normal_.cross(view_right_);

    // Set the view matrix
    set_view_matrix();
}

void CameraNode::set_perspective()
{
    // Get the dimensions at the near_clip clipping plane
    float h = near_clip_ * std::tan(degrees_to_radians(fov_ * 0.5f));
    float w = aspect_ratio_ * h;

    // Set the elements of the perspective projection matrix
    proj_.m00() = near_clip_ / w;
    proj_.m01() = 0.0f;
    proj_.m02() = 0.0f;
    proj_.m03() = 0.0f;
    proj_.m10() = 0.0f;
    proj_.m11() = near_clip_ / h;
    proj_.m12() = 0.0f;
    proj_.m13() = 0.0f;
    proj_.m20() = 0.0f;
    proj_.m21() = 0.0f;
    proj_.m22() = -(far_clip_ + near_clip_) / (far_clip_ - near_clip_);
    proj_.m23() = -(2.0f * far_clip_ * near_clip_) / (far_clip_ - near_clip_);
    proj_.m30() = 0.0f;
    proj_.m31() = 0.0f;
    proj_.m32() = -1.0f;
    proj_.m33() = 0.0f;
}

void CameraNode::set_view_matrix()
{
    // Set the view matrix using the view axes and the translation
    Vector3 t_vec(-vrp_.x, -vrp_.y, -vrp_.z);

    view_.m00() = view_right_.x;
    view_.m01() = view_right_.y;
    view_.m02() = view_right_.z;
    view_.m03() = view_right_.dot(t_vec);
    view_.m10() = view_up_.x;
    view_.m11() = view_up_.y;
    view_.m12() = view_up_.z;
    view_.m13() = view_up_.dot(t_vec);
    view_.m20() = view_normal_.x;
    view_.m21() = view_normal_.y;
    view_.m22() = view_normal_.z;
    view_.m23() = view_normal_.dot(t_vec);
    view_.m30() = 0.0f;
    view_.m31() = 0.0f;
    view_.m32() = 0.0f;
    view_.m33() = 1.0f;
}

} // namespace cg
