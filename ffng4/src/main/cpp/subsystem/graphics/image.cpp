#include "subsystem/graphics.h"
#include "image.h"

void Image::init() {
    m_graphics.get().regImage(this);
}

Image::Image(Image&& other) noexcept : m_graphics(other.m_graphics), m_texture(std::move(other.m_texture)) {
    m_graphics.get().regImageMove(&other, this);
}

Image& Image::operator=(Image&& other) noexcept {
    m_graphics = other.m_graphics;
    m_texture = std::move(other.m_texture);
    m_graphics.get().regImageMove(&other, this);
    return *this;
}

Image::~Image() noexcept {
    m_graphics.get().unregImage(this);
}

PNGImage::PNGImage(Graphics& graphics, std::string filename) : Image(graphics), m_filename(std::move(filename)) {
    init();
}

void PNGImage::renderTexture() {
    m_texture = m_graphics.get().loadPNG(m_filename);
}