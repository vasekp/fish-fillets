#include "subsystem/graphics.h"
#include "image.h"

Image::Image(Graphics& graphics, std::string filename) : m_graphics(graphics), m_filename(std::move(filename)), m_texture() {
    graphics.regImage(this);
}

Image::Image(Image&& other) : m_graphics(other.m_graphics), m_filename(std::move(other.m_filename)), m_texture(std::move(other.m_texture)) {
    m_graphics.get().regImageMove(&other, this);
}

Image& Image::operator=(Image&& other) {
    m_graphics = other.m_graphics;
    m_filename = std::move(other.m_filename);
    m_texture = std::move(other.m_texture);
    m_graphics.get().regImageMove(&other, this);
    return *this;
}

Image::~Image() {
    m_graphics.get().unregImage(this);
}

void Image::renderTexture() {
    m_texture = m_graphics.get().loadPNG(m_filename);
}