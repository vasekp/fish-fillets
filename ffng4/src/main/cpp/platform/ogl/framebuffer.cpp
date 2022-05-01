#include "platform/ogl.h"

namespace ogl {

    Framebuffer::Framebuffer(GLuint width, GLuint height) :
            m_texture(Texture::empty(width, height)) {
        glGenFramebuffers(1, &m_name);
        LOGV("framebuffer: generate %d", m_name);
        glBindFramebuffer(GL_FRAMEBUFFER, m_name);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_texture.bind();
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

}