#include "subsystem/graphics.h"

Image::Image(const SystemFile& file) :
    m_impl(std::make_shared<Impl>(file))
{ }

void Image::reload(Instance& instance) {
    if(!m_impl->m_texture.live())
        m_impl->m_texture = instance.graphics().loadImage(m_impl->m_file);
}