//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    glsl_shader.hpp
//	Purpose: Support for loadng a shader from a file and compiling it
//
//============================================================================

#ifndef __SHADER_SUPPORT_GLSL_SHADER_HPP__
#define __SHADER_SUPPORT_GLSL_SHADER_HPP__

#include "filesystem_support/file_loader.hpp"
#include "scene/graphics.hpp"

namespace cg
{

/**
 * Base shader class. Loads from file. Checks compile status.
 */
class GLSLShader
{
  public:
    GLSLShader() = delete;

    GLSLShader(const std::string &shader_str, GLenum shader_type);

    /**
     * Create shader from source code chararacter array.
     * @param  source  Source code for the shader.
     * @return  Returns true if successful, false if not.
     */
    bool create_from_source(const char *source);

    /**
     * Create shader from source code file.
     * @param  filename File name for the source code for the shader.
     * @return  Returns true if successful, false if not.
     */
    bool create(const char *filename);

    /**
     * Get the shader handle.
     * @return Returns a handle to the shader.
     */
    GLuint get() const;

  protected:
    std::string shader_type_str_;
    GLenum      gl_shader_type_;
    GLuint      gl_shader_;

    /**
     * Check the compile status of a shader.
     * @return  Returns true if the compile status is success.
     */
    bool check_compile_status(GLuint shader);

    // Utility to read a shader source file
    bool read_shader_source(const char *filename, FileContents &file_contents);

    /**
     * Logs a shader compile error
     */
    void log_compile_error(GLuint shader);
};

/**
 * GLSL vertex shader
 */
class GLSLVertexShader : public GLSLShader
{
  public:
    GLSLVertexShader();
};

/**
 * GLSL fragment shader
 */
class GLSLFragmentShader : public GLSLShader
{
  public:
    GLSLFragmentShader();
};

} // namespace cg

#endif
