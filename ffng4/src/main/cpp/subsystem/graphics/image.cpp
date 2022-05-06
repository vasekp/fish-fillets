#include "subsystem/graphics.h"

Image::Image(std::string filename) :
    m_filename(std::move(filename)),
    m_texture()
{ }

void Image::reload(Instance& instance) {
    if(!m_texture.live())
        m_texture = instance.graphics().loadImage(m_filename);
}