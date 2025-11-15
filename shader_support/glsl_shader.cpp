#include "shader_support/glsl_shader.hpp"

#include "filesystem_support/file_locator.hpp"

#include <iostream>

namespace cg
{

GLSLShader::GLSLShader(const std::string &shader_str, GLenum shader_type)
{
    shader_type_str_ = shader_str;
    gl_shader_type_ = shader_type;
}

bool GLSLShader::create_from_source(const char *source)
{
    bool success = true;
    gl_shader_ = glCreateShader(gl_shader_type_);
    glShaderSource(gl_shader_, 1, &source, NULL);
    glCompileShader(gl_shader_);
    if(!check_compile_status(gl_shader_))
    {
        std::cout << shader_type_str_ << " shader compile failed.\n";
        log_compile_error(gl_shader_);
        std::cout << shader_type_str_ << " Shader Source = \n";
        std::cout << source << '\n';
        success = false;
    }
    return success;
}

bool GLSLShader::create(const char *filename)
{
    FileContents file_contents;
    if(!read_shader_source(filename, file_contents)) exit(-1);

    uint32_t end_idx = file_contents.size - 1;
    while(file_contents.data[end_idx] < ' ' || file_contents.data[end_idx] > '~')
    {
        file_contents.data[end_idx] = '\0';
        --end_idx;
    }

    bool success = create_from_source(file_contents.data);
    file_contents.destroy();
    return success;
}

GLuint GLSLShader::get() const { return gl_shader_; }

bool GLSLShader::check_compile_status(GLuint shader)
{
    int param = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &param);
    return (param == GL_TRUE);
}

bool GLSLShader::read_shader_source(const char *filename, FileContents &file_contents)
{
    if(filename == 0)
    {
        std::cout << "NULL filename for shader...exiting\n";
        return false;
    }

    // Get path to file and file size
    auto file_info = locate_path_for_filename(filename);
    if(!file_info.found)
    {
        std::cout << "Could not find shader file " << filename
                  << ". Also not in parent directories\n";
        return false;
    }

    return load_file_contents(file_info.file_path, file_contents);
}

void GLSLShader::log_compile_error(GLuint shader)
{
    GLint   len = 0;
    GLsizei slen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if(len > 1)
    {
        GLchar *compilerlog = (GLchar *)new GLchar *[len];
        glGetShaderInfoLog(shader, len, &slen, compilerlog);
        std::cout << "compiler log:\n" << compilerlog << '\n';
        delete[] compilerlog;
    }
}

GLSLVertexShader::GLSLVertexShader() : GLSLShader("Vertex", GL_VERTEX_SHADER) {}

GLSLFragmentShader::GLSLFragmentShader() : GLSLShader("Fragment", GL_FRAGMENT_SHADER) {}

} // namespace cg
