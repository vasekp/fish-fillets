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

    public:
        std::int32_t width;
        std::int32_t height;

        Display(ANativeWindow *window);
        ~Display();
        void swap() const;
    };

    class Texture {
        GLuint texture;

    public:
        Texture() {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) : texture(other.texture) {
            other.texture = 0;
        }

        Texture& operator=(Texture&& other) {
            std::swap(texture, other.texture);
            return *this;
        }

        ~Texture() {
            if(texture)
                glDeleteTextures(1, &texture);
        }

        operator GLuint() const {
            return texture;
        }
    };

    class Image {
    private:
        Texture _texture;
        std::uint32_t _width;
        std::uint32_t _height;
    public:

        Image(const uint32_t width, const uint32_t height, const size_t stride, void* data) :
                _texture(), _width(width), _height(height)
        {
            assert(stride == 4 * width); // GLES does not support GL_UNPACK_ROW_LENGTH
            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        Image(Image&&) = default;

        const Texture& texture() const { return _texture; }
        auto width() const { return _width; }
        auto height() const { return _height; }
    };

    class Shader {
        GLuint handle;

    public:
        Shader(GLenum type, const std::string& code);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        ~Shader();

        operator GLuint() const {
            return handle;
        }
    };

    class Program {
        GLuint handle;

    public:
        Program(const Shader& vertexShader, const Shader& fragmentShader);
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;
        ~Program();

        operator GLuint() const {
            return handle;
        }
    };
}

#endif //FISH_FILLETS_OGL_H