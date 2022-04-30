#include "ogl-texture.h"

namespace ogl {

    Texture::Texture(GLuint width, GLuint height) : _width(width), _height(height) {
        glGenTextures(1, &name);
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &name);
    }

    Texture Texture::fromImageData(GLuint width, GLuint height, std::size_t stride, void *data) {
        assert(stride == 4 * width); // GLESv2 does not support GL_UNPACK_ROW_LENGTH
        Texture ret{width, height};
        ret.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) width, (GLsizei) height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        return ret;
    }

    Texture Texture::empty(GLuint width, GLuint height) {
        Texture ret{width, height};
        ret.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) width, (GLsizei) height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        return ret;
    }

    Texture::Texture(Texture &&other) noexcept :
        _width(other._width),
        _height(other._height),
        name(other.name)
    {
        other.name = 0;
    }

    Texture& Texture::operator=(Texture &&other) noexcept {
        _width = other._width;
        _height = other._height;
        std::swap(name, other.name);
        return *this;
    }

    void Texture::bind() {
        glBindTexture(GL_TEXTURE_2D, name);
    }

}
