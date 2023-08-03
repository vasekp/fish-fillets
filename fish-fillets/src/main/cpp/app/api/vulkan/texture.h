#ifndef FISH_FILLETS_VULKAN_TEXTURE_H
#define FISH_FILLETS_VULKAN_TEXTURE_H

#include "subsystem/graphics/texturetype.h"

namespace vulkan {

struct TextureImpl;

class Texture {
    std::unique_ptr<TextureImpl> pImpl;
    std::uint32_t m_width;
    std::uint32_t m_height;

public:
    Texture(vulkan::Display& display, std::uint32_t width, std::uint32_t height, TextureType type, std::uint8_t* data);

    Texture(Texture&&);
    Texture& operator=(Texture&&);
    ~Texture();

    auto width() const { return m_width; }
    auto height() const { return m_height; }

    const vk::Image& image() const;
    const vk::ImageView& imageView() const;
    operator const vk::Image&() const { return image(); }
    const vk::DescriptorSet& descriptorSet() const;

    void replaceData(std::uint8_t* data);
};

}

#endif //FISH_FILLETS_VULKAN_TEXTURE_H
