#ifndef FISH_FILLETS_OGL_SHADER_H
#define FISH_FILLETS_OGL_SHADER_H

namespace ogl {

    class Shader {
    public:
        Shader() = default;
        Shader(const ogl::Display& display, GLenum type, const std::string& code);
        Shader(Shader&&) noexcept = default;
        ~Shader();

        operator GLuint() const { return m_name; }

    private:
        std::weak_ptr<int> m_ref;
        util::ResetOnMove<GLuint, 0> m_name;
    };

}

#endif //FISH_FILLETS_OGL_SHADER_H
