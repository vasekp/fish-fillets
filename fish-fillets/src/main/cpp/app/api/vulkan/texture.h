#ifndef FISH_FILLETS_VULKAN_TEXTURE_H
#define FISH_FILLETS_VULKAN_TEXTURE_H

#include "subsystem/graphics/texturetype.h"

namespace vulkan {

struct TextureImpl;

class Texture {
    std::unique_ptr<TextureImpl> pImpl;
    USize m_size;

public:
    Texture(vulkan::Display& display, USize size, TextureType type, std::uint8_t* data);

    Texture(Texture&&);
    Texture& operator=(Texture&&);
    ~Texture();

    auto size() const { return m_size; }

    const vk::Image& image() const;
    const vk::ImageView& imageView() const;
    operator const vk::Image&() const { return image(); }
    const vk::DescriptorSet& descriptorSet() const;

    void replaceData(std::uint8_t* data);
};

}

#endif //FISH_FILLETS_VULKAN_TEXTURE_H
