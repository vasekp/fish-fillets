#include "subsystem/graphics.h"

TextureTarget::TextureTarget(GraphicsSystem& system) :
    DrawTarget(system),
    m_texture{system, system.display().size()},
    m_framebuffer{system.display(), m_texture.native()},
    m_size()
{ }

void TextureTarget::bind() {
    m_framebuffer.bind();
}

void TextureTarget::resize(unsigned width, unsigned height, float scale) { // TODO log, phys?
    m_size = {width, height};
    auto texSize = (scale * m_size).round();
    if(texSize != m_texture.physSize())
        m_texture = Texture(m_system, m_size, texSize);
    m_framebuffer.texture(m_texture.native());
}
