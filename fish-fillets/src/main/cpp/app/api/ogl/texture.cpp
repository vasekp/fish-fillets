#include "api/ogl.h"

namespace ogl {

    Texture::Texture(const std::weak_ptr<int>& ref, GLuint width, GLuint height) : m_ref(ref), m_width(width), m_height(height) {
        glGenTextures(1, &m_name);
        Log::verbose<Log::graphics>("texture: generate ", m_name);
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    Texture::~Texture() {
        if(m_name && !m_ref.expired()) {
            Log::verbose<Log::graphics>("texture: delete ", m_name);
            glDeleteTextures(1, &m_name);
        }
    }

    Texture Texture::fromImageData(const ogl::Display& display, GLuint width, GLuint height, int channels, void *data) {
        assert(channels == 1 || channels == 4);
        auto format = channels == 4 ? GL_RGBA : GL_LUMINANCE;
        Texture ret{display.ref(), width, height};
        ret.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, format, (GLsizei) width, (GLsizei) height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        return ret;
    }

    Texture Texture::empty(const ogl::Display& display, GLuint width, GLuint height) {
        Texture ret{display.ref(), width, height};
        ret.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) width, (GLsizei) height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        return ret;
    }

    Texture::Texture(Texture&& other) noexcept :
        m_ref(std::move(other.m_ref)),
        m_name(other.m_name),
        m_width(other.m_width),
        m_height(other.m_height)
    {
        other.m_name = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        std::swap(m_ref, other.m_ref);
        std::swap(m_name, other.m_name);
        m_width = other.m_width;
        m_height = other.m_height;
        return *this;
    }

    void Texture::bind() const {
        glBindTexture(GL_TEXTURE_2D, m_name);
    }

}
