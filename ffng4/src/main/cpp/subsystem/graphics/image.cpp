#include "subsystem/graphics.h"
#include "image.h"

Image::Image(SystemFile file) : m_file(std::move(file)), m_texture() { }

Image::Image(const SystemFile& file, Instance& instance) : Image(file) {
    reload(instance);
}

void Image::reload(Instance& instance) {
    if(!m_texture.live())
        m_texture = instance.graphics().loadPNG(m_file);
}
