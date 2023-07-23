#ifndef FISH_FILLETS_VULKAN_FRAMEBUFFER_H
#define FISH_FILLETS_VULKAN_FRAMEBUFFER_H

namespace vulkan {

class Framebuffer {
    const vulkan::Display& m_display;
    vk::raii::Framebuffer m_native;

public:
    Framebuffer(const vulkan::Display& display, const Texture& texture);

    void bind();
    void texture(const Texture& texture);

private:
    vk::raii::Framebuffer createFramebuffer(const vulkan::Display& display, const Texture& texture);
};

}

#endif // FISH_FILLETS_VULKAN_FRAMEBUFFER_H
