#include "api/ogl.h"

namespace ogl {

    Program::Program(const std::shared_ptr<ogl::Display>& ref, const Shader &vertexShader, const Shader &fragmentShader) : m_ref(ref) {
        m_name = glCreateProgram();
        Log::verbose("program: generate ", m_name);
        if (!m_name)
            Log::fatal("glCreateProgram failed");

        glAttachShader(m_name, vertexShader);
        glAttachShader(m_name, fragmentShader);
        glBindAttribLocation(m_name, aPosition, "aPosition");
        glLinkProgram(m_name);

        int status;
        glGetProgramiv(m_name, GL_LINK_STATUS, &status);

        if (status == GL_FALSE) {
            int loglen, actlen;
            glGetProgramiv(m_name, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetProgramInfoLog(m_name, loglen, &actlen, error.data());
            glDeleteProgram(m_name);
            Log::fatal("glLinkProgram failed: ", error);
        }
    }

    Program::Program(Program&& other) noexcept :
            m_name(other.m_name),
            m_uniforms(std::move(other.m_uniforms))
    {
        other.m_name = 0;
    }

    Program& Program::operator=(Program&& other) noexcept {
        std::swap(m_name, other.m_name);
        std::swap(m_uniforms, other.m_uniforms);
        return *this;
    }

    Program::~Program() {
        if(m_name && !m_ref.expired()) {
          Log::verbose("program: delete ", m_name);
            glDeleteProgram(m_name);
        }
    }

    GLint Program::uniform(const std::string &ident) const {
        auto entry = m_uniforms.find(ident);
        if (entry == m_uniforms.end()) {
            glUseProgram(m_name);
            return m_uniforms[ident] = glGetUniformLocation(m_name, ident.c_str());
        } else
            return entry->second;
    }

}
