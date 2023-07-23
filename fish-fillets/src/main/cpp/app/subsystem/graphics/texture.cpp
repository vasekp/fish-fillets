#include "subsystem/graphics.h"

Texture::Texture(ogl::Texture&& native) :
    m_native(std::move(native)),
    m_physSize{(int)m_native.width(), (int)m_native.height()},
    m_logSize(m_physSize)
{ }

Texture::Texture(GraphicsSystem& system, FCoords logSize, ICoords physSize) :
    m_native(ogl::Texture::empty(system.display(), physSize.x, physSize.y)),
    m_physSize(physSize),
    m_logSize(logSize)
{ }

Texture::Texture(GraphicsSystem& system, void *data, ICoords size, int channels) :
    Texture(ogl::Texture::fromImageData(system.display(), size.x, size.y, channels * size.x, data, channels))
{ }

void Texture::bind() const {
    m_native.bind();
}
