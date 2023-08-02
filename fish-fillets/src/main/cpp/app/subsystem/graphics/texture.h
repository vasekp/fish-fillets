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
    ICoords m_physSize;
    FCoords m_logSize;

public:
    Texture() = delete;
    Texture(Texture&& other) = default;
    Texture& operator=(Texture&& other) = default;

    Texture(GraphicsSystem& system, ICoords size);
    Texture(GraphicsSystem& system, FCoords logSize, ICoords physSize);
    Texture(GraphicsSystem& system, std::uint8_t* data, ICoords size, TextureType = TextureType::image);
    Texture(GraphicsSystem& system, std::uint8_t* data, FCoords logSize, ICoords physSize, TextureType = TextureType::image);

    FCoords logSize() const { return m_logSize; }
    ICoords physSize() const { return m_physSize; }
    const PlatformType& native() const { return m_native; }

    void replaceData(std::uint8_t* data);

private:
    Texture(PlatformType&& texture, FCoords logSize);
};

#endif //FISH_FILLETS_TEXTURE_H
