#include "subsystem/graphics.h"

Texture::Texture(PlatformType&& native, FCoords logSize) :
    m_native(std::move(native)),
    m_physSize{(int)m_native.width(), (int)m_native.height()},
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsSystem& system, TextureType type, ICoords physSize, FCoords logSize, std::uint8_t* data) :
    Texture{PlatformType(system.display(), physSize.x, physSize.y, type, data), logSize}
{ }

Texture::Texture(GraphicsSystem& system, TextureType type, ICoords size, std::uint8_t* data) :
    Texture(system, type, size, size, data)
{ }

void Texture::replaceData(std::uint8_t* data) {
    m_native.replaceData(data);
}
