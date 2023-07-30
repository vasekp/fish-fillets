#include "subsystem/graphics.h"

Texture::Texture(PlatformType&& native) :
    m_native(std::move(native)),
    m_physSize{(int)m_native.width(), (int)m_native.height()},
    m_logSize(m_physSize)
{ }

Texture::Texture(GraphicsSystem& system, FCoords logSize, ICoords physSize) :
    m_native(PlatformType::empty(system.display(), physSize.x, physSize.y)),
    m_physSize(physSize),
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsSystem& system, ICoords size) :
    Texture(system, size, size)
{ }

Texture::Texture(GraphicsSystem& system, void *data, ICoords size, TextureType type) :
    Texture(PlatformType::fromImageData(system.display(), size.x, size.y, type, data))
{ }
