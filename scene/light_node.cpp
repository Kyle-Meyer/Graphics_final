#include "scene/light_node.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

LightNode::LightNode(uint32_t idx)
{
    node_type_ = SceneNodeType::LIGHT;

    index_ = idx;

    // Set default attenuation to 1 and spotlight to false
    enabled_ = false;
    const_atten_ = 1.0f;
    lin_atten_ = 0.0f;
    quad_atten_ = 0.0f;
    is_spotlight_ = false;

    // Note: color constructors default rgb to 0 and alpha to 1
}

void LightNode::enable() { enabled_ = true; }

void LightNode::disable() { enabled_ = false; }

void LightNode::set_ambient(const Color4 &c) { ambient_ = c; }

void LightNode::set_diffuse(const Color4 &c) { diffuse_ = c; }

void LightNode::set_specular(const Color4 &c) { specular_ = c; }

void LightNode::set_position(const HPoint3 &pos)
{
    position_ = pos;

    // If directional light - make sure the position represents a unit vector
    if(position_.w == 0.0f)
    {
        Vector3 L(position_.x, position_.y, position_.z);
        L.normalize();
        position_.x = L.x;
        position_.y = L.y;
        position_.z = L.z;
    }
}

void LightNode::set_spotlight(const Vector3 &dir, const float exp, const float cutoff)
{
    spot_direction_ = dir;
    spot_exponent_ = exp;
    spot_cutoff_ = std::cos(degrees_to_radians(cutoff));
    is_spotlight_ = true;
}

void LightNode::set_spotlight_direction(const Vector3 &dir) { spot_direction_ = dir; }

void LightNode::turn_off_spotlight() { is_spotlight_ = false; }

void LightNode::set_attenuation(float constant, float linear, float quadratic)
{
    const_atten_ = constant;
    lin_atten_ = linear;
    quad_atten_ = quadratic;
}

void LightNode::draw(SceneState &scene_state)
{
    glUniform1i(scene_state.lights[index_].enabled, static_cast<int>(enabled_));
    if(enabled_)
    {
        glUniform1i(scene_state.lights[index_].spotlight, static_cast<int>(is_spotlight_));
        glUniform4fv(scene_state.lights[index_].position, 1, &position_.x);
        glUniform4fv(scene_state.lights[index_].ambient, 1, &ambient_.r);
        glUniform4fv(scene_state.lights[index_].diffuse, 1, &diffuse_.r);
        glUniform4fv(scene_state.lights[index_].specular, 1, &specular_.r);
        glUniform1f(scene_state.lights[index_].att_constant, const_atten_);
        glUniform1f(scene_state.lights[index_].att_linear, lin_atten_);
        glUniform1f(scene_state.lights[index_].att_quadratic, quad_atten_);
        if(is_spotlight_)
        {
            // Note we use cos of the spotlight cutoff angle so we don't have
            // to compute cos in the shader
            glUniform1f(scene_state.lights[index_].spot_cutoff, spot_cutoff_);
            glUniform3fv(scene_state.lights[index_].spot_direction, 1, &spot_direction_.x);
            glUniform1f(scene_state.lights[index_].spot_exponent, spot_exponent_);
        }

        // Track the maximum light index that is enabled
        if(index_ >= (uint32_t)scene_state.max_enabled_light)
        {
            glUniform1i(scene_state.lightcount_loc, index_ + 1);
            scene_state.max_enabled_light = index_;
        }
    }

    // Draw children of this node
    SceneNode::draw(scene_state);

    // To be proper we should disable this light so it does not impact any nodes that
    // are not descended from this node
    glUniform1i(scene_state.lights[index_].enabled, 0);
}

} // namespace cg
