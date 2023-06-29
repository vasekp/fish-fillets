#include "api/ogl.h"

namespace ogl {

    Shader::Shader(const std::shared_ptr<ogl::Display>& ref, GLenum type, const std::string& code) : m_ref(ref) {
        m_name = glCreateShader(type);
        Log::verbose("shader: generate ", m_name);
        if (!m_name)
            Log::fatal("glCreateShader failed");

        const auto source = code.c_str();
        glShaderSource(m_name, 1, &source, nullptr);
        glCompileShader(m_name);

        int status;
        glGetShaderiv(m_name, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            int loglen, actlen;
            glGetShaderiv(m_name, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetShaderInfoLog(m_name, loglen, &actlen, error.data());
            glDeleteShader(m_name);
            Log::fatal("glCompileShader failed: ", error);
        }
    }

    Shader::Shader(Shader&& other) noexcept : m_name(other.m_name) {
        other.m_name = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        std::swap(m_name, other.m_name);
        return *this;
    }

    Shader::~Shader() {
        if(m_name && !m_ref.expired()) {
            Log::verbose("shader: delete ", m_name);
            glDeleteShader(m_name);
        }
    }

}
