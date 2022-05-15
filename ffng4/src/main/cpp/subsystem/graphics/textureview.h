#ifndef FISH_FILLETS_GRAPHICS_TEXTURE_VIEW_H
#define FISH_FILLETS_GRAPHICS_TEXTURE_VIEW_H

#include "common.h"
#include "platform/ogl.h"

class TextureView {
    const ogl::Texture& m_texture;
    FCoords m_size;

public:
    TextureView(const ogl::Texture& texture, const FCoords &size) : m_texture(texture), m_size(size) { }
    TextureView(const ogl::Texture& texture) : TextureView(texture, {texture.width(), texture.height()}) { }
    TextureView(const Image& image) : TextureView(image.texture()) { }

    const auto& texture() const { return m_texture; }
    auto size() const { return m_size; }
    int width() const { return m_size.x(); }
    int height() const { return m_size.y(); }
    float fwidth() const { return m_size.fx(); }
    float fheight() const { return m_size.fy(); }
};

#endif //FISH_FILLETS_GRAPHICS_TEXTURE_VIEW_H
