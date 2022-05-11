#ifndef FISH_FILLETS_GRAPHICS_OFFSCREENTARGET_H
#define FISH_FILLETS_GRAPHICS_OFFSCREENTARGET_H

class OffscreenTarget : public DrawTarget {
    const std::shared_ptr<ogl::Display> &m_ref;
    ogl::Framebuffer m_framebuffer;
    ogl::Texture m_texture;

public:
    OffscreenTarget(const std::shared_ptr<ogl::Display> &ref) : m_ref(ref), m_framebuffer(ref), m_texture() { }

    void bind() const override;
    const auto& texture() const { return m_texture; }

protected:
    void resize(unsigned int width, unsigned int height) override;
};

#endif //FISH_FILLETS_GRAPHICS_OFFSCREENTARGET_H