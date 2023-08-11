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

public:
    TextureTarget(GraphicsSystem& system, USize physSize, FCoords logSize = {});

    const PlatformType& framebuffer() const { return m_framebuffer; }
    const Texture& texture() const { return m_texture; }
    FCoords size() const override { return m_texture.logSize(); }
    void resize(USize physSize, FCoords logSize = {});
    void bind() override;
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
