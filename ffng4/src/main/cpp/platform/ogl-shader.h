#ifndef FISH_FILLETS_OGL_SHADER_H
#define FISH_FILLETS_OGL_SHADER_H

namespace ogl {

    class Shader {
        GLuint name;

    public:
        Shader() = default;
        Shader(GLenum type, const std::string &code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;
        ~Shader();

        operator GLuint() const { return name; }
    };

}

#endif //FISH_FILLETS_OGL_SHADER_H