#include "platform/ogl.h"

namespace ogl {

    Framebuffer::Framebuffer() {
        glGenFramebuffers(1, &m_name);
        LOGV("framebuffer: generate %d", m_name);
    }

    Framebuffer::Framebuffer(GLuint width, GLuint height) : Framebuffer() {
        m_texture = Texture::empty(width, height);
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Framebuffer::~Framebuffer() {
        if(m_name)
            LOGV("framebuffer: delete %d", m_name);
        glDeleteFramebuffers(1, &m_name);
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_name);
    }

    void Framebuffer::bindWith(const Texture& texture) const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_name);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    }

    Color Framebuffer::getPixel(unsigned x, unsigned y) const {
        std::uint8_t pixels[4];
        glReadPixels((GLint)x, (GLint)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        return {pixels[0], pixels[1], pixels[2], pixels[3]};
    }

}