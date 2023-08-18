#include "subsystem/graphics.h"
#include "graphicsbackend.h"
#include "texture.h"

Texture::Texture(BACKEND::Texture&& native, FCoords logSize) :
    m_native(std::move(native)),
    m_physSize(m_native.size()),
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsBackend& backend, TextureType type, USize physSize, FCoords logSize, std::uint8_t* data) :
    Texture{BACKEND::Texture(backend.display(), physSize, type, data), logSize}
{ }

Texture::Texture(GraphicsBackend& backend, TextureType type, USize size, std::uint8_t* data) :
    Texture(backend, type, size, FCoords{size}, data)
{ }

void Texture::replaceData(std::uint8_t* data) {
    m_native.replaceData(data);
}
