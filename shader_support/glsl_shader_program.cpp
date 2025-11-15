#include "shader_support/glsl_shader_program.hpp"

#include <iostream>

namespace cg
{

GLSLShaderProgram::GLSLShaderProgram() : shader_program_(0) {}
GLSLShaderProgram::~GLSLShaderProgram() {}

void GLSLShaderProgram::create() { shader_program_ = glCreateProgram(); }

bool GLSLShaderProgram::attach_shaders(GLuint vertex_shader, GLuint fragment_shader)
{
    glAttachShader(shader_program_, vertex_shader);
    glAttachShader(shader_program_, fragment_shader);
    glLinkProgram(shader_program_);
    if(!check_link_status())
    {
        std::cout << "Shader link failed\n";
        log_link_error();
        return false;
    }
    return true;
}

GLuint GLSLShaderProgram::get_program() const { return shader_program_; }

void GLSLShaderProgram::use() { glUseProgram(shader_program_); }

bool GLSLShaderProgram::check_link_status()
{
    int param = 0;
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &param);
    return (param == GL_TRUE);
}

void GLSLShaderProgram::log_link_error()
{
    GLint   len = 0;
    GLsizei slen = 0;
    glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &len);
    if(len > 1)
    {
        GLchar *linklog = (GLchar *)new GLchar *[len];
        glGetProgramInfoLog(shader_program_, len, &slen, linklog);
        std::cout << "Program Link Log:\n" << linklog << '\n';
        delete[] linklog;
    }
}

} // namespace cg
