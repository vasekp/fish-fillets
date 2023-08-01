#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
protected:
    std::reference_wrapper<Instance> m_instance;
    std::optional<Texture> m_texture;

    Image(Instance& instance) : m_instance(instance), m_texture() { }
    Image(Image&&) noexcept;
    Image& operator=(Image&&) noexcept;
    void init(); // Must be called at the end of derived classes' constructors.

public:
    virtual ~Image() noexcept;

    const auto& texture() const { return *m_texture; }
    FCoords size() const { return m_texture->logSize(); }

    virtual void render() = 0;

private:
    friend class Graphics;
};

class PNGImage : public Image {
    std::string m_filename;
    TextureType m_type;

public:
    PNGImage(Instance& instance, std::string filename, TextureType type = TextureType::image);
    PNGImage(PNGImage&&) = default;
    PNGImage& operator=(PNGImage&&) = default;

    auto filename() const { return m_filename; }
    void render() override;
};

class IFont;

class TextImage : public Image {
    std::reference_wrapper<IFont> m_font;
    std::string m_text;

public:
    TextImage(Instance& instance, IFont& font, std::string text);
    TextImage(TextImage&&) = default;
    TextImage& operator=(TextImage&&) = default;

    IFont& font() { return m_font.get(); }
    const std::string& text() const { return m_text; }
    void render() override;
};

struct ImageData {
    unsigned width;
    unsigned height;
    std::unique_ptr<std::uint8_t[]> data;
};

class BufferImage : public Image {
    ICoords m_size;
    TextureType m_type;
    std::unique_ptr<std::uint8_t[]> m_data;

public:
    BufferImage(Instance& instance, ICoords size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data);
    BufferImage(BufferImage&&) = default;
    BufferImage& operator=(BufferImage&&) = default;

    void render() override;
    void replace(std::unique_ptr<std::uint8_t[]>&& data);
    void compose(ImageData& picture, ICoords origin);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
