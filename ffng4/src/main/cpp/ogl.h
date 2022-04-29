#ifndef FISH_FILLETS_OGL_H
#define FISH_FILLETS_OGL_H

#include "common.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace ogl {

    class Display {
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        std::int32_t _width;
        std::int32_t _height;

    public:

        Display(ANativeWindow *window);
        Display(const Display&) = delete;
        Display& operator=(const Display&) = delete;
        ~Display();

        auto width() const { return _width; }
        auto height() const { return _height; }

        void bind() const;
        void swap() const;
    };

    class Texture {
        GLuint _width;
        GLuint _height;
        GLuint name{};

    public:
        static Texture fromImageData(GLuint width, GLuint height, std::size_t stride, void* data);
        static Texture empty(GLuint width, GLuint height);

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;
        ~Texture();

        operator GLuint() const { return name; }
        auto width() const { return _width; }
        auto height() const { return _height; }

        void bind() const;

    private:
        Texture(GLuint width, GLuint height);
    };

    class Shader {
        GLuint name;

    public:
        Shader(GLenum type, const std::string& code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        ~Shader();

        operator GLuint() const { return name; }
    };

    class Program {
        GLuint name;

    public:
        Program(const Shader& vertexShader, const Shader& fragmentShader);
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;
        ~Program();

        operator GLuint() const { return name; }
    };

    class Framebuffer {
        GLuint name;
        Texture _texture;
        GLuint _width;
        GLuint _height;

    public:
        Framebuffer(GLuint maxWidth, GLuint maxHeight);
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        operator GLuint() const { return name; }
        auto width() const { return _width; }
        auto height() const { return _height; }
        auto texWidth() const { return _texture.width(); }
        auto texHeight() const { return _texture.height(); }
        auto& texture() const { return _texture; }

        void bind() const;
        void resize(GLuint width, GLuint height);
    };
}

#endif //FISH_FILLETS_OGL_H