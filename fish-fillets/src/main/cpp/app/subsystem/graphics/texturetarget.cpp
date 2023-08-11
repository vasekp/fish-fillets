#include "subsystem/graphics.h"
#include "graphicssystem.h"

struct TextureTarget::Impl {
    Texture texture;
    Platform::Framebuffer framebuffer;
};

TextureTarget::TextureTarget(GraphicsSystem& system, USize physSize, FCoords logSize) :
    DrawTarget(system)
{
    Texture texture{system, TextureType::image, physSize, logSize ? logSize : FCoords{physSize}, nullptr};
    Platform::Framebuffer framebuffer{system.display(), texture.native()};
    pImpl = std::make_unique<Impl>(std::move(texture), std::move(framebuffer));
}

TextureTarget::~TextureTarget() = default;

const Platform::Framebuffer& TextureTarget::framebuffer() const {
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
        pImpl->texture = Texture(m_system, TextureType::image, physSize, logSize, nullptr);
    else
        pImpl->texture.logSize() = logSize;
    pImpl->framebuffer.texture(pImpl->texture.native());
}
