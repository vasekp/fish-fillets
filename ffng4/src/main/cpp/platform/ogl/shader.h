#ifndef FISH_FILLETS_OGL_SHADER_H
#define FISH_FILLETS_OGL_SHADER_H

namespace ogl {

    class Shader {
        GLuint name = 0;

    public:
        Shader() = default;
        Shader(GLenum type, const std::string &code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;
        ~Shader();

        operator GLuint() const { return name; }
        void invalidate() { if(name) LOGV("shader: detach %d", name); name = 0; }
    };

}

#endif //FISH_FILLETS_OGL_SHADER_H