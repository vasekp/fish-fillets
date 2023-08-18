#ifndef FISH_FILLETS_TEXTURE_H
#define FISH_FILLETS_TEXTURE_H

#include "graphicsbackend.h"

class Texture {
public:
    Texture() = delete;
    Texture(Texture&& other) noexcept = default;
    Texture& operator=(Texture&& other) noexcept = default;

    Texture(GraphicsBackend& backend, TextureType type, USize size, std::uint8_t* data);
    Texture(GraphicsBackend& backend, TextureType type, USize physSize, FCoords logSize, std::uint8_t* data);

    FCoords logSize() const { return m_logSize; }
    FCoords& logSize() { return m_logSize; }
    USize physSize() const { return m_physSize; }
    const BACKEND::Texture& native() const { return m_native; }

    void replaceData(std::uint8_t* data);

private:
    BACKEND::Texture m_native;
    USize m_physSize;
    FCoords m_logSize;

    Texture(BACKEND::Texture&& texture, FCoords logSize);
};

#endif //FISH_FILLETS_TEXTURE_H
