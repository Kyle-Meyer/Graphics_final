#include "scene/scene.hpp"

#include <iostream>

namespace cg
{

void check_error(const char *str)
{
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) std::cout << str << ": OpenGL Error: " << err << '\n';
    switch(err)
    {
        case(GL_NO_ERROR): return;
        case(GL_INVALID_ENUM):
            std::cout << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated "
                         "argument. The offending command is ignored and has no other side effect "
                         "than to set the error flag.\n";
            break;
        case(GL_INVALID_VALUE):
            std::cout
                << "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is "
                   "ignored and has no other side effect than to set the error flag.\n";
            break;
        case(GL_INVALID_OPERATION):
            std::cout << "GL_INVALID_OPERATION: The specified operation is not allowed in the "
                         "current state. The offending command is ignored and has no other side "
                         "effect than to set the error flag.\n";
            break;
        case(GL_OUT_OF_MEMORY):
            std::cout << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the "
                         "command. The state of the GL is undefined, except for the state of the "
                         "error flags, after this error is recorded.\n";
            break;
        case(GL_INVALID_FRAMEBUFFER_OPERATION):
            std::cout
                << "GL_INVALID_FRAMEBUFFER_OPERATION: Framebuffer is not framebuffer complete.\n";
            break;
        default: std::cout << "Unknown Error: Not mapped.\n"; break;
    }
}

} // namespace cg
