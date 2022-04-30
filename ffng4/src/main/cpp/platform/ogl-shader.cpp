#include "ogl-shader.h"

namespace ogl {

    Shader::Shader(GLenum type, const std::string& code) {
        name = glCreateShader(type);
        if (!name)
            ::error("glCreateShader failed");

        const auto source = code.c_str();
        glShaderSource(name, 1, &source, nullptr);
        glCompileShader(name);

        int status;
        glGetShaderiv(name, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            int loglen, actlen;
            glGetShaderiv(name, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetShaderInfoLog(name, loglen, &actlen, error.data());
            glDeleteShader(name);
            ::error("glCompileShader failed", "%s", error.c_str());
        }
    }

    Shader::~Shader() {
        glDeleteShader(name);
    }

}