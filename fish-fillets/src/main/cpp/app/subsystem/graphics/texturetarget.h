#ifndef FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
#define FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H

class TextureTarget : public DrawTarget {
public:
    TextureTarget(GraphicsBackend& backend, USize physSize, FCoords logSize = {});
    ~TextureTarget();

    const BACKEND::Framebuffer& framebuffer() const;
    const Texture& texture() const;
    FCoords size() const override;
    void resize(USize physSize, FCoords logSize = {});
    void bind() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
