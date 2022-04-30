#include "ogl-program.h"

namespace ogl {

    Program::Program(const Shader &vertexShader, const Shader &fragmentShader) {
        name = glCreateProgram();
        if (!name)
            ::error("glCreateProgram failed");

        glAttachShader(name, vertexShader);
        glAttachShader(name, fragmentShader);
        glBindAttribLocation(name, aPosition, "aPosition");
        glLinkProgram(name);

        int status;
        glGetProgramiv(name, GL_LINK_STATUS, &status);

        if (status == GL_FALSE) {
            int loglen, actlen;
            glGetProgramiv(name, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetProgramInfoLog(name, loglen, &actlen, error.data());
            glDeleteProgram(name);
            ::error("glLinkProgram failed", "%s", error.c_str());
        }
    }

    Program::~Program() {
        glDeleteProgram(name);
    }

    GLint Program::uniform(const std::string &ident) {
        auto entry = uniforms.find(ident);
        if (entry == uniforms.end()) {
            glUseProgram(name);
            return uniforms[ident] = glGetUniformLocation(name, ident.c_str());
        } else
            return entry->second;
    }

}