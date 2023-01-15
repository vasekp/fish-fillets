#include "subsystem/graphics.h"

void TextureTarget::bind() const {
    m_framebuffer.bind();
    glViewport(0, 0, (GLsizei)m_texture.width(), (GLsizei)m_texture.height());
}

void TextureTarget::resize(unsigned width, unsigned height) {
    if(width != m_texture.width() || height != m_texture.height())
        m_texture = ogl::Texture(ogl::Texture::empty(m_ref, width, height));
    m_size = {width, height};
    m_framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

FCoords TextureTarget::size() const {
    return m_size;
}