#include "subsystem/graphics.h"

TextureTarget::TextureTarget(GraphicsSystem& system, ICoords physSize, FCoords logSize) :
    DrawTarget(system),
    m_texture{system, TextureType::image, physSize, logSize ? logSize : physSize, nullptr},
    m_framebuffer{system.display(), m_texture.native()}
{ }

void TextureTarget::bind() {
    m_framebuffer.bind();
}

void TextureTarget::resize(ICoords physSize, FCoords logSize) {
    if(!logSize)
        logSize = physSize;
    if(physSize != m_texture.physSize())
        m_texture = Texture(m_system, TextureType::image, physSize, logSize, nullptr);
    else
        m_texture.logSize() = logSize;
    m_framebuffer.texture(m_texture.native());
}
