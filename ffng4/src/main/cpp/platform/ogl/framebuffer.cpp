#include "platform/ogl.h"

namespace ogl {

    Framebuffer::Framebuffer(const std::shared_ptr<ogl::Display>& ref) : m_ref(ref) {
        glGenFramebuffers(1, &m_name);
        LOGV("framebuffer: generate %d", m_name);
    }

    Framebuffer::~Framebuffer() {
        if(m_name && !m_ref.expired()) {
            LOGV("framebuffer: delete %d", m_name);
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