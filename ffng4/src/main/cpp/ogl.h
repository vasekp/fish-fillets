#ifndef FISH_FILLETS_OGL_H
#define FISH_FILLETS_OGL_H

#include "common.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

namespace ogl {

    struct Display {
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        std::int32_t width;
        std::int32_t height;

    public:
        Display(ANativeWindow *window);
        ~Display();
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

        ~Texture() {
            glDeleteTextures(1, &texture);
        }

        operator GLuint() const {
            return texture;
        }

        GLuint id() const {
            return texture;
        }
    };

    struct Image {
        const std::uint32_t width;
        const std::uint32_t height;
        const std::size_t stride;
        const Texture texture;

        Image(const uint32_t width, const uint32_t height, const size_t stride, void* data) :
                width(width), height(height), stride(stride), texture()
        {
            assert(stride == 4 * width); // GLES does not support GL_UNPACK_ROW_LENGTH
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        GLuint texId() const {
            return texture;
        }
    };
}

#endif //FISH_FILLETS_OGL_H