#include "subsystem/graphics.h"
#include "graphicsbackend.h"

struct TextureTarget::Impl {
    Texture texture;
    BACKEND::Framebuffer framebuffer;
};

TextureTarget::TextureTarget(GraphicsBackend& backend, USize physSize, FCoords logSize) :
    DrawTarget(backend)
{
    Texture texture{backend, TextureType::image, physSize, logSize ? logSize : FCoords{physSize}, nullptr};
    BACKEND::Framebuffer framebuffer{backend.display(), texture.native()};
    pImpl = std::make_unique<Impl>(std::move(texture), std::move(framebuffer));
}

TextureTarget::~TextureTarget() = default;

const BACKEND::Framebuffer& TextureTarget::framebuffer() const {
    return pImpl->framebuffer;
}

const Texture& TextureTarget::texture() const {
    return pImpl->texture;
}

FCoords TextureTarget::size() const {
    return pImpl->texture.logSize();
}

void TextureTarget::bind() {
    pImpl->framebuffer.bind();
}

void TextureTarget::resize(USize physSize, FCoords logSize) {
    if(!logSize)
        logSize = FCoords{physSize};
    if(physSize != pImpl->texture.physSize())
        pImpl->texture = Texture(m_backend, TextureType::image, physSize, logSize, nullptr);
    else
        pImpl->texture.logSize() = logSize;
    pImpl->framebuffer.texture(pImpl->texture.native());
}
