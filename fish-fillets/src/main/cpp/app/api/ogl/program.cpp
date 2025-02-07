#include "api/ogl.h"

namespace ogl {

    Program::Program(const ogl::Display& display, const Shader& vertexShader, const Shader& fragmentShader) : m_ref(display.ref()) {
        m_name = glCreateProgram();
        Log::verbose<Log::graphics>("program: generate ", m_name);
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

    Program::~Program() {
        if(m_name && !m_ref.expired()) {
          Log::verbose<Log::graphics>("program: delete ", m_name);
            glDeleteProgram(m_name);
        }
    }

    GLint Program::uniform(const std::string& ident) const {
        auto entry = m_uniforms.find(ident);
        if (entry == m_uniforms.end()) {
            glUseProgram(m_name);
            return m_uniforms[ident] = glGetUniformLocation(m_name, ident.c_str());
        } else
            return entry->second;
    }

}
