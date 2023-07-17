#ifndef FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
#define FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H

#include "drawtarget.h"

class TextureTarget : public DrawTarget {
    const std::shared_ptr<ogl::Display> &m_ref;
    ogl::Framebuffer m_framebuffer;
    Texture m_texture;
    FCoords m_size;

public:
    TextureTarget(GraphicsSystem& system);

    const Texture& texture() const { return m_texture; }
    void resize(unsigned width, unsigned height, float scale = 1.f);
    FCoords size() const override;

private:
    void bind() override;
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
