#include "subsystem/graphics.h"
#include "image.h"

void Image::init() {
    m_instance.get().graphics().regImage(this);
}

Image::Image(Image&& other) noexcept : m_instance(other.m_instance), m_texture(std::move(other.m_texture)) {
    m_instance.get().graphics().regImageMove(&other, this);
}

Image& Image::operator=(Image&& other) noexcept {
    m_instance = other.m_instance;
    m_texture = std::move(other.m_texture);
    m_instance.get().graphics().regImageMove(&other, this);
    return *this;
}

Image::~Image() noexcept {
    m_instance.get().graphics().unregImage(this);
}

PNGImage::PNGImage(Instance& instance, std::string filename, TextureType type) :
    Image(instance), m_filename(std::move(filename)), m_type(type)
{
    init();
}

void PNGImage::render() {
    m_texture = decoders::png(m_instance, m_filename, m_type);
}

TextImage::TextImage(Instance& instance, IFont& font, std::string text) :
    Image(instance), m_font(font), m_text(std::move(text))
{
    init();
}

void TextImage::render() {
    m_texture = m_font.get().renderText(m_text);
}

BufferImage::BufferImage(Instance& instance, ICoords size, TextureType type, void* data) :
    Image(instance), m_size(size), m_type(type)
{
    auto byteSize = size.x * size.y * type.channels();
    m_data.resize(byteSize);
    if(data)
        std::memcpy(m_data.data(), data, byteSize);
    init();
}

void BufferImage::render() {
    m_texture = Texture(m_instance.get().graphics().system(), m_data.data(), m_size, m_type);
}

void BufferImage::replace(void* data) {
    std::memcpy(m_data.data(), data, m_data.size());
    render();
}
