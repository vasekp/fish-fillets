#include "subsystem/graphics.h"

TextureTarget::TextureTarget(GraphicsSystem& system) :
    DrawTarget(system),
    m_texture{system, TextureType::image, system.display().size(), nullptr},
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
        m_texture = Texture(m_system, TextureType::image, texSize, m_size, nullptr);
    m_framebuffer.texture(m_texture.native());
}
