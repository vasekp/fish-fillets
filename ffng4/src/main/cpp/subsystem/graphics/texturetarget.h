#ifndef FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
#define FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H

class TextureTarget : public DrawTarget {
    const std::shared_ptr<ogl::Display> &m_ref;
    ogl::Framebuffer m_framebuffer;
    ogl::Texture m_texture;
    FCoords m_size;

public:
    TextureTarget(const std::shared_ptr<ogl::Display> &ref) : m_ref(ref), m_framebuffer(ref), m_texture(), m_size() { }

    void bind() const override;
    TextureView texture() const { return {m_texture, m_size}; }
    void resize(unsigned width, unsigned height);
    void resize(unsigned width, unsigned height, float widthScale, float heightScale);
    FCoords size() const override;
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H