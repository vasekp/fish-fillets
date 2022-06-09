#include "subsystem/graphics.h"
#include "subsystem/files.h"

ogl::Texture Graphics::loadPNG(const std::string& filename) const {
    return ogl::Texture::empty(m_system->ref(), 1, 1);
}
