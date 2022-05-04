#include "subsystem/graphics.h"

void Image::reload(Instance& instance) {
    if(!m_texture.live())
        m_texture = instance.graphics().loadImage(m_filename);
}