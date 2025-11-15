//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    shader_node.hpp
//	Purpose: Scene graph shader node.
//
//============================================================================

#ifndef __SCENE_SHADER_NODE_HPP__
#define __SCENE_SHADER_NODE_HPP__

#include "scene/scene_node.hpp"

#include "shader_support/glsl_shader.hpp"
#include "shader_support/glsl_shader_program.hpp"

namespace cg
{

/**
 * Shader node. Enables a shader program. The program is loaded with
 * different constructor methods. Derived shader node classes should
 * provide specialization to control uniforms and attributes.
 */
class ShaderNode : public SceneNode
{
  public:
    /**
     * Constructor.
     */
    ShaderNode();

    /**
     * Destructor.
     */
    virtual ~ShaderNode();

    /**
     * Create a shader program given a filename for the vertex shader and a filename
     * for the fragment shader.
     * @param  vertex_shader_filename    Vertex shader file name
     * @param  fragment_shader_filename  Fragment shader file name
     * @return  Returns true if successful, false if compile or link errors occur.
     */
    bool create(const char *vertex_shader_filename, const char *fragment_shader_filename);

    /**
     * Create a shader program given source char array for the vertex shader and source
     * for the fragment shader.
     * @param  vertex_shader_source    Vertex shader source (char array)
     * @param  fragment_shader_source  Fragment shader source (char array)
     * @return  Returns true if successful, false if compile or link errors occur.
     */
    bool create_from_source(const char *vertex_shader_source, const char *fragment_shader_source);

    // Derived classes must add this to set all internal uniforms and attribute locations
    virtual bool get_locations() = 0;

  protected:
    GLSLVertexShader   vertex_shader_;
    GLSLFragmentShader fragment_shader_;
    GLSLShaderProgram  shader_program_;
};

} // namespace cg

#endif
