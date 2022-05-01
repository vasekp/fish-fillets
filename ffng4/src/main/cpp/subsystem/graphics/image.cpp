#include "subsystem/graphics.h"

Image::~Image() {
    if(m_ref.expired())
        m_texture.invalidate();
}

void Image::reload(Instance* instance) {
    m_texture.invalidate();
    m_ref = {instance->graphics};
    m_texture = instance->graphics->loadImage(m_filename);
}