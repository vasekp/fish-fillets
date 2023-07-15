#include "subsystem/graphics.h"

TextureTarget::TextureTarget(GraphicsSystem& system) :
    DrawTarget(system),
    m_ref(system.ref()),
    m_framebuffer(system.ref()),
    m_texture(),
    m_size()
{ }

TextureView TextureTarget::texture() const {
    return {m_texture, m_size};
}

void TextureTarget::bind() {
    m_framebuffer.bind();
    glViewport(0, 0, (GLsizei)m_texture.width(), (GLsizei)m_texture.height());
}

void TextureTarget::resize(unsigned width, unsigned height, float scale) {
    m_size = {width, height};
    auto texSize = scale * m_size;
    if(texSize.x() != (int)m_texture.width() || texSize.y() != (int)m_texture.height())
        m_texture = ogl::Texture(ogl::Texture::empty(m_ref, texSize.x(), texSize.y()));
    m_framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

FCoords TextureTarget::size() const {
    return m_size;
}
