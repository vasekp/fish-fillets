#include "subsystem/graphics.h"
#include "graphicssystem.h"
#include "texture.h"

Texture::Texture(Platform::Texture&& native, FCoords logSize) :
    m_native(std::move(native)),
    m_physSize(m_native.size()),
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsSystem& system, TextureType type, USize physSize, FCoords logSize, std::uint8_t* data) :
    Texture{Platform::Texture(system.display(), physSize, type, data), logSize}
{ }

Texture::Texture(GraphicsSystem& system, TextureType type, USize size, std::uint8_t* data) :
    Texture(system, type, size, FCoords{size}, data)
{ }

void Texture::replaceData(std::uint8_t* data) {
    m_native.replaceData(data);
}
