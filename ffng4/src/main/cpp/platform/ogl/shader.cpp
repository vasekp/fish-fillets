#include "platform/ogl.h"

namespace ogl {

    Shader::Shader(GLenum type, const std::string& code) {
        name = glCreateShader(type);
        LOGV("shader: generate %d", name);
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

    Shader::Shader(Shader&& other) noexcept : name(other.name) {
        other.name = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        std::swap(name, other.name);
        return *this;
    }

    Shader::~Shader() {
        if(name)
            LOGV("shader: delete %d", name);
        glDeleteShader(name);
    }

}