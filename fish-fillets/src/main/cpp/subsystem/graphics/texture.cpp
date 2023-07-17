#include "subsystem/graphics.h"

Texture::Texture(ogl::Texture&& native) :
    m_native(std::move(native)),
    m_physSize{(int)m_native.width(), (int)m_native.height()},
    m_logSize(m_physSize)
{ }

Texture::Texture(const std::shared_ptr<ogl::Display>& ref, FCoords logSize, ICoords physSize) :
    m_native(ogl::Texture::empty(ref, physSize.x, physSize.y)),
    m_physSize(physSize),
    m_logSize(logSize)
{ }

Texture::Texture(const std::shared_ptr<ogl::Display>& ref, void *data, ICoords size, int channels) :
    Texture(ogl::Texture::fromImageData(ref, size.x, size.y, channels * size.x, data, channels))
{ }

void Texture::bind() const {
    m_native.bind();
}
