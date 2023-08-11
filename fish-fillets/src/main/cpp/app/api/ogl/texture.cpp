#include "api/ogl.h"

namespace ogl {

    Texture::Texture(const ogl::Display& display, USize size, TextureType type, std::uint8_t* data) :
        m_ref(display.ref()), m_size(size), m_channels(type.channels())
    {
        glGenTextures(1, &m_name);
        Log::verbose<Log::graphics>("texture: generate ", m_name);
        bind();
        if(data)
            replaceData(data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_size.width, (GLsizei)m_size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        auto filter = type == TextureType::mask ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    Texture::~Texture() {
        if(m_name && !m_ref.expired()) {
            Log::verbose<Log::graphics>("texture: delete ", m_name);
            glDeleteTextures(1, &m_name);
        }
    }

    Texture::Texture(Texture&& other) noexcept :
        m_ref(std::move(other.m_ref)),
        m_name(other.m_name),
        m_size(other.m_size),
        m_channels(other.m_channels)
    {
        other.m_name = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        std::swap(m_ref, other.m_ref);
        std::swap(m_name, other.m_name);
        m_size = other.m_size;
        m_channels = other.m_channels;
        return *this;
    }

    void Texture::bind() const {
        glBindTexture(GL_TEXTURE_2D, m_name);
    }

    void Texture::replaceData(std::uint8_t* data) {
        auto format = m_channels == 4 ? GL_RGBA : GL_LUMINANCE;
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, format, (GLsizei)m_size.width, (GLsizei)m_size.height, 0, format, GL_UNSIGNED_BYTE, data);
    }

}
