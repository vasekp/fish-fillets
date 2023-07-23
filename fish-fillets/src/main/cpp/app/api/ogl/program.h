#ifndef FISH_FILLETS_OGL_PROGRAM_H
#define FISH_FILLETS_OGL_PROGRAM_H

namespace ogl {

    class Program {
        std::weak_ptr<int> m_ref;
        GLuint m_name = 0;
        mutable std::map<std::string, GLint> m_uniforms;

    public:
        Program() = default;
        Program(const ogl::Display& display, const Shader& vertexShader, const Shader& fragmentShader);
        Program(const Program&) = delete;
        Program &operator=(const Program&) = delete;
        Program(Program&&) noexcept;
        Program& operator=(Program&&) noexcept;
        ~Program();

        operator GLuint() const { return m_name; }

        GLint uniform(const std::string &ident) const;

        static constexpr GLint aPosition = 0;
    };

}

#endif //FISH_FILLETS_OGL_PROGRAM_H
