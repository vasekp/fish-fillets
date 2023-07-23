#ifndef FISH_FILLETS_VULKAN_TEXTURE_H
#define FISH_FILLETS_VULKAN_TEXTURE_H

namespace vulkan {

struct TextureImpl;

class Texture {
    std::unique_ptr<TextureImpl> pImpl;
    std::uint32_t m_width;
    std::uint32_t m_height;

public:
    static Texture empty(const Display& display, std::uint32_t width, std::uint32_t height);
    static Texture fromImageData(const Display& display, std::uint32_t width, std::uint32_t height, int channels, void *data);

    Texture(Texture&&);
    Texture& operator=(Texture&&);
    ~Texture();

    auto width() const { return m_width; }
    auto height() const { return m_height; }
    const vk::Image& image() const;
    const vk::ImageView& imageView() const;

private:
    Texture(std::uint32_t width, std::uint32_t height, vk::raii::Image&& image, vk::raii::DeviceMemory&& memory,
            vk::raii::ImageView&& imageView/*, vk::raii::Sampler&& sampler*/);
};

}

#endif //FISH_FILLETS_VULKAN_TEXTURE_H
