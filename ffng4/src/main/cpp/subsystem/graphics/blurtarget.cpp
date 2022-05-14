#include "subsystem/graphics.h"

void BlurTarget::bind() const {
    m_framebuffer.bind();
    glViewport(0, 0, (GLsizei)m_texture.width(), (GLsizei)m_texture.height());
}

void BlurTarget::resize(unsigned int width, unsigned int height) {
    m_texture = ogl::Texture(ogl::Texture::empty(m_ref, width, height));
    m_framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
}