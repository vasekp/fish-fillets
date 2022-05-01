#ifndef FISH_FILLETS_OGL_SHADER_H
#define FISH_FILLETS_OGL_SHADER_H

namespace ogl {

    class Shader {
        GLuint m_name = 0;

    public:
        Shader() = default;
        Shader(GLenum type, const std::string &code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;
        ~Shader();

        operator GLuint() const { return m_name; }
        void invalidate() { if(m_name) LOGV("shader: detach %d", m_name); m_name = 0; }
    };

}

#endif //FISH_FILLETS_OGL_SHADER_H