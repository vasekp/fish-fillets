#ifndef FISH_FILLETS_GRAPHICS_BLURTARGET_H
#define FISH_FILLETS_GRAPHICS_BLURTARGET_H

class BlurTarget : public DrawTarget {
    const std::shared_ptr<ogl::Display> &m_ref;
    ogl::Framebuffer m_framebuffer;
    ogl::Texture m_texture;

public:
    BlurTarget(const std::shared_ptr<ogl::Display> &ref) : m_ref(ref), m_framebuffer(ref), m_texture() { }

    void bind() const override;
    const auto& texture() const { return m_texture; }

protected:
    void resize(unsigned int width, unsigned int height) override;
};

#endif //FISH_FILLETS_GRAPHICS_BLURTARGET_H