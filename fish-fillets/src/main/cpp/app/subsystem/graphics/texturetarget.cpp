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

void TextureTarget::resize(ICoords physSize, FCoords logSize) {
    if(!logSize)
        logSize = physSize;
    m_size = logSize;
    if(physSize != m_texture.physSize())
        m_texture = Texture(m_system, TextureType::image, physSize, logSize, nullptr);
    m_framebuffer.texture(m_texture.native());
}
