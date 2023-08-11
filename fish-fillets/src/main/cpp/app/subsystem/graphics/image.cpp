#include "subsystem/graphics.h"
#include "image.h"
#include "texture.h"

Image::~Image() = default;

const Texture& Image::texture() const {
    return *m_texture;
}

FCoords Image::size() const {
    return m_texture->logSize();
}

PNGImage::PNGImage(std::string filename, TextureType type, Private) :
    m_filename(std::move(filename)), m_type(type)
{ }

ImageRef PNGImage::create(Instance& instance, std::string filename, TextureType type) {
    auto ptr = std::make_unique<PNGImage>(std::move(filename), type, Private::tag);
    return instance.graphics().addImage(std::move(ptr));
}

void PNGImage::render(Instance& instance) {
    auto [size, data] = decoders::png(instance, m_filename);
    m_texture = std::make_unique<Texture>(instance.graphics().system(), m_type, size, data.get());
}

TextImage::TextImage(IFont& font, std::string text, Private) :
    m_font(font), m_text(std::move(text))
{
    m_font.regImage(this);
}

TextImage::~TextImage() {
    m_font.unregImage(this);
}

ImageRef TextImage::create(Instance& instance, IFont& font, std::string text) {
    auto ptr = std::make_unique<TextImage>(font, text, Private::tag);
    return instance.graphics().addImage(std::move(ptr));
}

void TextImage::render(Instance& instance) {
    m_texture = m_font.renderText(instance, m_text);
}

BufferImage::BufferImage(USize size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data, Private) :
    m_size(size), m_type(type), m_data(std::move(data))
{ }

ImageRef BufferImage::create(Instance& instance, USize size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data) {
    auto ptr = std::make_unique<BufferImage>(size, type, std::move(data), Private::tag);
    return instance.graphics().addImage(std::move(ptr));
}

void BufferImage::render(Instance& instance) {
    m_texture = std::make_unique<Texture>(instance.graphics().system(), m_type, m_size, m_data.get());
}

void BufferImage::replace(std::unique_ptr<std::uint8_t[]>&& data) {
    swap(m_data, data);
    if(m_texture)
        m_texture->replaceData(m_data.get());
}

void BufferImage::compose(ImageData& picture, ICoords origin) {
    assert(m_type == TextureType::image);
    if(!origin.within({}, ICoords{m_size, 1} - ICoords{picture.size})) {
        Log::error("BufferImage::compose picture does not fit");
        return;
    }
    using rgba = std::array<std::uint8_t, 4>;
    auto src = reinterpret_cast<rgba*>(picture.data.get());
    auto dst = reinterpret_cast<rgba*>(m_data.get());
    rgba* srcStart = src;
    rgba* dstStart = dst + origin.y * m_size.width + origin.x;
    for(auto y = 0u; y < picture.size.height; y++) {
        rgba *srcPtr = srcStart, *dstPtr = dstStart;
        for(auto x = 0u; x < picture.size.width; x++) {
            if((*srcPtr)[3] != 0)
                *dstPtr = *srcPtr;
            srcPtr++;
            dstPtr++;
        }
        dstStart += m_size.width;
        srcStart += picture.size.width;
    }
    if(m_texture)
        m_texture->replaceData(m_data.get());
}

ImageRef::ImageRef(Instance& instance, const std::unique_ptr<Image>& ptr) :
    m_instance(instance), m_image(ptr.get())
{ }

ImageRef::ImageRef(ImageRef&& other) noexcept : m_instance(other.m_instance), m_image(other.m_image) {
    other.m_image = nullptr;
}

ImageRef& ImageRef::operator=(ImageRef&& other) noexcept {
    std::swap(m_instance, other.m_instance);
    std::swap(m_image, other.m_image);
    return *this;
}

ImageRef::~ImageRef() {
    if(m_image != nullptr)
        m_instance.get().graphics().deleteImage(*this);
}
