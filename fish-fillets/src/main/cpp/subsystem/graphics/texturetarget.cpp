#include "subsystem/graphics.h"

TextureTarget::TextureTarget(GraphicsSystem& system) :
    DrawTarget(system),
    m_framebuffer(system.ref()),
    m_texture(),
    m_size()
{ }

void TextureTarget::bind() {
    m_framebuffer.bind();
    auto size = m_texture.physSize();
    glViewport(0, 0, size.x, size.y);
}

void TextureTarget::resize(unsigned width, unsigned height, float scale) { // TODO log, phys?
    m_size = {width, height};
    auto texSize = (scale * m_size).round();
    if(texSize != m_texture.physSize())
        m_texture = Texture(m_system, m_size, texSize);
    m_framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.native(), 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

FCoords TextureTarget::size() const {
    return m_size;
}
