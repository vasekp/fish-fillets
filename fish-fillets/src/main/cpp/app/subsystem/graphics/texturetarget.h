#ifndef FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
#define FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H

class TextureTarget : public DrawTarget {
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformType = vulkan::Framebuffer;
#else
    using PlatformType = ogl::Framebuffer;
#endif

    Texture m_texture;
    PlatformType m_framebuffer;
    FCoords m_size;

public:
    TextureTarget(GraphicsSystem& system);

    const PlatformType& framebuffer() const { return m_framebuffer; }
    const Texture& texture() const { return m_texture; }
    FCoords size() const override { return m_size; }
    void resize(unsigned width, unsigned height, float scale = 1.f);
    void bind() override;
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
