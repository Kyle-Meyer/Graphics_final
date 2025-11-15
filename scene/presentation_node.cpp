#include "scene/presentation_node.hpp"
#include "scene/image_data.hpp"

#include <iostream>

namespace cg
{

PresentationNode::PresentationNode() :
    texture_id_(0), has_texture_(false), use_texture_(false)
{
    node_type_ = SceneNodeType::PRESENTATION;
    material_shininess_ = 1.0f;
}

PresentationNode::PresentationNode(const Color4 &ambient,
                                   const Color4 &diffuse,
                                   const Color4 &specular,
                                   const Color4 &emission,
                                   float         shininess) :
    material_ambient_(ambient),
    material_diffuse_(diffuse),
    material_specular_(specular),
    material_emission_(emission),
    material_shininess_(shininess),
    texture_id_(0),
    has_texture_(false),
    use_texture_(false)
{
    node_type_ = SceneNodeType::PRESENTATION;
}

PresentationNode::~PresentationNode()
{
    // Clean up texture if one was loaded
    if(has_texture_)
    {
        glDeleteTextures(1, &texture_id_);
    }
}

void PresentationNode::set_material_ambient(const Color4 &c) { material_ambient_ = c; }

void PresentationNode::set_material_diffuse(const Color4 &c) { material_diffuse_ = c; }

void PresentationNode::set_material_ambient_and_diffuse(const Color4 &c)
{
    material_ambient_ = c;
    material_diffuse_ = c;
}

void PresentationNode::set_material_specular(const Color4 &c) { material_specular_ = c; }

void PresentationNode::set_material_emission(const Color4 &c) { material_emission_ = c; }

void PresentationNode::set_material_shininess(float s) { material_shininess_ = s; }

// NEW: Load texture from file
bool PresentationNode::load_texture(const std::string &filename, bool use_mipmaps)
{
    // Load image data using existing function
    ImageData img_data;
    load_image_data(img_data, filename, true); // Load as RGB

    if(img_data.data == nullptr)
    {
        std::cout << "PresentationNode: Failed to load texture: " << filename << '\n';
        return false;
    }

    // Generate texture
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    // Upload texture data
    GLenum format = (img_data.channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 img_data.w,
                 img_data.h,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 img_data.data);

    // Generate mipmaps if requested
    if(use_mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set filtering with mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        // Set filtering without mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Set wrapping mode (repeat for tiling)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Clean up
    free_image_data(img_data);
    glBindTexture(GL_TEXTURE_2D, 0);

    has_texture_ = true;
    use_texture_ = true;

    std::cout << "PresentationNode: Successfully loaded texture: " << filename << '\n';
    return true;
}

// NEW: Enable or disable texture
void PresentationNode::enable_texture(bool enable)
{
    if(has_texture_)
    {
        use_texture_ = enable;
    }
}

// NEW: Bind texture
void PresentationNode::bind_texture()
{
    if(has_texture_ && use_texture_)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }
}

void PresentationNode::draw(SceneState &scene_state)
{
    // Set the material uniform values
    glUniform4fv(scene_state.material_ambient_loc, 1, &material_ambient_.r);
    glUniform4fv(scene_state.material_diffuse_loc, 1, &material_diffuse_.r);
    glUniform4fv(scene_state.material_specular_loc, 1, &material_specular_.r);
    glUniform4fv(scene_state.material_emission_loc, 1, &material_emission_.r);
    glUniform1f(scene_state.material_shininess_loc, material_shininess_);

    // NEW: Set texture uniforms
    glUniform1i(scene_state.use_texture_loc, use_texture_ && has_texture_);
    if(use_texture_ && has_texture_)
    {
        bind_texture();
        glUniform1i(scene_state.texture_sampler_loc, 0); // Texture unit 0
    }

    // Draw children of this node
    SceneNode::draw(scene_state);
}

} // namespace cg
