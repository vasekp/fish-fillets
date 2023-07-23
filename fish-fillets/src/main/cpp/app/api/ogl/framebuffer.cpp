#include "api/ogl.h"

namespace ogl {

    Framebuffer::Framebuffer(const ogl::Display& display, const ogl::Texture& texture) :
        m_ref(display.ref()),
        m_texture(&texture)
    {
        glGenFramebuffers(1, &m_name);
        Log::verbose<Log::graphics>("framebuffer: generate ", m_name);
    }

    Framebuffer::~Framebuffer() {
        if(m_name && !m_ref.expired()) {
          Log::verbose<Log::graphics>("framebuffer: delete ", m_name);
            glDeleteFramebuffers(1, &m_name);
        }
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_name);
        glViewport(0, 0, m_texture->width(), m_texture->height());
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
