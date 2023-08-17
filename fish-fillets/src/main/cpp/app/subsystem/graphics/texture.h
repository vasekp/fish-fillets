#ifndef FISH_FILLETS_TEXTURE_H
#define FISH_FILLETS_TEXTURE_H

class GraphicsSystem;

class Texture {
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformType = vulkan::Texture;
#else
    using PlatformType = ogl::Texture;
#endif

    PlatformType m_native;
    USize m_physSize;
    FCoords m_logSize;

public:
    Texture() = delete;
    Texture(Texture&& other) noexcept = default;
    Texture& operator=(Texture&& other) noexcept = default;

    Texture(GraphicsSystem& system, TextureType type, USize size, std::uint8_t* data);
    Texture(GraphicsSystem& system, TextureType type, USize physSize, FCoords logSize, std::uint8_t* data);

    FCoords logSize() const { return m_logSize; }
    FCoords& logSize() { return m_logSize; }
    USize physSize() const { return m_physSize; }
    const PlatformType& native() const { return m_native; }

    void replaceData(std::uint8_t* data);

private:
    Texture(PlatformType&& texture, FCoords logSize);
};

#endif //FISH_FILLETS_TEXTURE_H
