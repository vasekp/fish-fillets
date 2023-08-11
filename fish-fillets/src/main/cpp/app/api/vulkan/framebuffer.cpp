#include "../vulkan.h"

namespace vulkan {

Framebuffer::Framebuffer(const vulkan::Display& display, const Texture& texture) :
    m_display(display),
    m_native(createFramebuffer(display, texture))
{ }

void Framebuffer::bind() {
    std::unreachable();
}

void Framebuffer::texture(const Texture& texture) {
    Log::verbose<Log::graphics>("Framebuffer texture: ", texture.size());
    m_native = createFramebuffer(m_display, texture);
}

vk::raii::Framebuffer Framebuffer::createFramebuffer(const vulkan::Display& display, const Texture& texture) {
    Log::verbose<Log::graphics>("Framebuffer create: ", texture.size());
    return {display.device(), vk::FramebufferCreateInfo{}
        .setRenderPass(display.renderPass())
        .setAttachments(texture.imageView())
        .setWidth(texture.size().width)
        .setHeight(texture.size().height)
        .setLayers(1)};
}

}
