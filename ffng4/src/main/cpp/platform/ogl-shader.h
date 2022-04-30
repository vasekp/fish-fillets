#ifndef FISH_FILLETS_OGL_SHADER_H
#define FISH_FILLETS_OGL_SHADER_H

namespace ogl {

    class Shader {
        GLuint name;

    public:
        Shader(GLenum type, const std::string &code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        ~Shader();

        operator GLuint() const { return name; }
    };

}

#endif //FISH_FILLETS_OGL_SHADER_H