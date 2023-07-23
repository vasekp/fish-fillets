#include "api/ogl.h"

namespace ogl {

    Framebuffer::Framebuffer(const ogl::Display& display) : m_ref(display.ref()) {
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
    }

    void Framebuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
