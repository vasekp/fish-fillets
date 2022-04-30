#ifndef FISH_FILLETS_OGL_PROGRAM_H
#define FISH_FILLETS_OGL_PROGRAM_H

namespace ogl {

    class Program {
        GLuint name;
        std::map<std::string, GLint> uniforms;

    public:
        Program(const Shader& vertexShader, const Shader& fragmentShader);
        Program(const Program &) = delete;
        Program &operator=(const Program &) = delete;
        ~Program();

        operator GLuint() { return name; }

        GLint uniform(const std::string &ident);

        static constexpr GLint aPosition = 0;
    };

}

#endif //FISH_FILLETS_OGL_PROGRAM_H
