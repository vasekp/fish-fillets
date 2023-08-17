#ifndef FISH_FILLETS_OGL_PROGRAM_H
#define FISH_FILLETS_OGL_PROGRAM_H

namespace ogl {

    class Program {
        std::weak_ptr<int> m_ref;
        util::ResetOnMove<GLuint, 0> m_name;
        mutable std::map<std::string, GLint> m_uniforms;

    public:
        Program() = default;
        Program(const ogl::Display& display, const Shader& vertexShader, const Shader& fragmentShader);
        Program(Program&&) noexcept = default;
        ~Program();

        operator GLuint() const { return m_name; }

        GLint uniform(const std::string& ident) const;

        static constexpr GLint aPosition = 0;
    };

}

#endif //FISH_FILLETS_OGL_PROGRAM_H
