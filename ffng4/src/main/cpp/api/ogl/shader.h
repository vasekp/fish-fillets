#ifndef FISH_FILLETS_OGL_SHADER_H
#define FISH_FILLETS_OGL_SHADER_H

namespace ogl {

    class Shader {
        std::weak_ptr<const Display> m_ref;
        GLuint m_name = 0;

    public:
        Shader() = default;
        Shader(const std::shared_ptr<ogl::Display>& ref, GLenum type, const std::string &code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;
        ~Shader();

        operator GLuint() const { return m_name; }
    };

}

#endif //FISH_FILLETS_OGL_SHADER_H