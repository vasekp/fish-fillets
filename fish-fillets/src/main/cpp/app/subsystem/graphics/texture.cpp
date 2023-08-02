#include "subsystem/graphics.h"

Texture::Texture(PlatformType&& native, FCoords logSize) :
    m_native(std::move(native)),
    m_physSize{(int)m_native.width(), (int)m_native.height()},
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsSystem& system, FCoords logSize, ICoords physSize) :
    m_native(PlatformType::empty(system.display(), physSize.x, physSize.y)),
    m_physSize(physSize),
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsSystem& system, ICoords size) :
    Texture{system, size, size}
{ }

Texture::Texture(GraphicsSystem& system, std::uint8_t* data, ICoords size, TextureType type) :
    Texture{system, data, size, size, type}
{ }

Texture::Texture(GraphicsSystem& system, std::uint8_t* data, FCoords logSize, ICoords physSize, TextureType type) :
    Texture{PlatformType::fromImageData(system.display(), physSize.x, physSize.y, type, data), logSize}
{ }

void Texture::replaceData(std::uint8_t* data) {
    m_native.replaceData(data);
}
