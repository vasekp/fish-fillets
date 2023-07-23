#ifndef FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
#define FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H

#include "drawtarget.h"

class TextureTarget : public DrawTarget {
    Texture m_texture;
    ogl::Framebuffer m_framebuffer;
    FCoords m_size;

public:
    TextureTarget(GraphicsSystem& system);

    const Texture& texture() const { return m_texture; }
    FCoords size() const override { return m_size; }
    void resize(unsigned width, unsigned height, float scale = 1.f);
    void bind() override;
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_TARGET_H
