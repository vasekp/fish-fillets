#ifndef FISH_FILLETS_OGL_PROGRAM_H
#define FISH_FILLETS_OGL_PROGRAM_H

namespace ogl {

    class Program {
        GLuint name = 0;
        mutable std::map<std::string, GLint> uniforms;

    public:
        Program() = default;
        Program(const Shader& vertexShader, const Shader& fragmentShader);
        Program(const Program &) = delete;
        Program &operator=(const Program &) = delete;
        Program(Program&&) noexcept;
        Program& operator=(Program&&) noexcept;
        ~Program();

        operator GLuint() const { return name; }

        GLint uniform(const std::string &ident) const;
        void invalidate() { if(name) LOGV("program: detach %d", name); name = 0; }

        static constexpr GLint aPosition = 0;
    };

}

#endif //FISH_FILLETS_OGL_PROGRAM_H
