#include "api/ogl.h"

namespace ogl {

    Framebuffer::Framebuffer(const ogl::Display& display, const ogl::Texture& texture) :
        m_ref(display.ref())
    {
        glGenFramebuffers(1, &m_name);
        Log::verbose<Log::graphics>("framebuffer: generate ", m_name);
        this->texture(texture);
    }

    Framebuffer::Framebuffer(Framebuffer&& other) :
        m_ref(std::move(other.m_ref)),
        m_texture(other.m_texture),
        m_name(other.m_name)
    {
        other.m_name = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& other) {
        std::swap(m_ref, other.m_ref);
        std::swap(m_texture, other.m_texture);
        std::swap(m_name, other.m_name);
        return *this;
    }

    Framebuffer::~Framebuffer() {
        if(m_name && !m_ref.expired()) {
          Log::verbose<Log::graphics>("framebuffer: delete ", m_name);
            glDeleteFramebuffers(1, &m_name);
        }
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_name);
        glViewport(0, 0, m_texture->size().width, m_texture->size().height);
    }

    void Framebuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::texture(const ogl::Texture& texture) {
        m_texture = &texture;
        glBindFramebuffer(GL_FRAMEBUFFER, m_name);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *m_texture, 0);
    }

}
