#include "scene/shader_node.hpp"

#include <iostream>

namespace cg
{

ShaderNode::ShaderNode() { node_type_ = SceneNodeType::SHADER; }

ShaderNode::~ShaderNode() {}

bool ShaderNode::create(const char *vertex_shader_filename, const char *fragment_shader_filename)
{
    // Create and compile the vertex shader
    if(!vertex_shader_.create(vertex_shader_filename))
    {
        std::cout << "Vertex Shader compile failed\n";
        return false;
    }

    // Create and compile the fragment shader
    if(!fragment_shader_.create(fragment_shader_filename))
    {
        std::cout << "Fragment Shader compile failed\n";
        return false;
    }

    shader_program_.create();
    if(!shader_program_.attach_shaders(vertex_shader_.get(), fragment_shader_.get()))
    {
        std::cout << "Shader program link failed\n";
        return false;
    }
    return true;
}

bool ShaderNode::create_from_source(const char *vertex_shader_source,
                                    const char *fragment_shader_source)
{
    // Create and compile the vertex shader
    if(!vertex_shader_.create_from_source(vertex_shader_source))
    {
        std::cout << "Vertex Shader compile failed\n";
        return false;
    }

    // Create and compile the fragment shader
    if(!fragment_shader_.create_from_source(fragment_shader_source))
    {
        std::cout << "Fragment Shader compile failed\n";
        return false;
    }

    shader_program_.create();
    if(!shader_program_.attach_shaders(vertex_shader_.get(), fragment_shader_.get()))
    {
        std::cout << "Shader program link failed\n";
        return false;
    }
    return true;
}

} // namespace cg
