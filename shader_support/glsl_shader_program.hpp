//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    glsl_shader_program.hpp
//	Purpose: Support for GLSL shader programs
//============================================================================

#ifndef __SHADER_SUPPORT_GLSL_SHADER_PROGRAM_HPP__
#define __SHADER_SUPPORT_GLSL_SHADER_PROGRAM_HPP__

#include "scene/graphics.hpp"

namespace cg
{

/**
 * GLSL shader program
 */
class GLSLShaderProgram
{
  public:
    GLSLShaderProgram();
    ~GLSLShaderProgram();

    /**
     * Create a shader program
     */
    void create();

    /**
     * Attach the specified shaders.
     */
    bool attach_shaders(GLuint vertex_shader, GLuint fragment_shader);

    /**
     * Get the shader program handle
     * @return  Returns the handle to the shader program.
     */
    GLuint get_program() const;

    /**
     * Use this shader program.
     */
    void use();

  protected:
    GLuint shader_program_;

    /**
     * Checks the link status.
     * @return  Returns true if hte linker was successful, false if an error occured.
     */
    bool check_link_status();

    /**
     * Logs a shader program link error to the console window.
     */
    void log_link_error();
};

} // namespace cg

#endif
