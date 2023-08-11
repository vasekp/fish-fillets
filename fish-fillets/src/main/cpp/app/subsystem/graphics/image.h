#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
protected:
    std::optional<Texture> m_texture;

    Image() = default;
    Image(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) = delete;

    enum class Private { tag };

public:
    virtual ~Image() noexcept { }

    const auto& texture() const { return m_texture.value(); }
    FCoords size() const { return m_texture->logSize(); }

private:
    virtual void render(Instance& instance) = 0;

    friend class Graphics;
};

class ImageRef {
    std::reference_wrapper<Instance> m_instance;
    Image* m_image;

public:
    ImageRef(Instance& instance, const std::unique_ptr<Image>& ptr);
    ImageRef(const ImageRef&) = delete;
    ImageRef& operator=(const ImageRef&) = delete;
    ImageRef(ImageRef&&) noexcept;
    ImageRef& operator=(ImageRef&&) noexcept;
    ~ImageRef() noexcept;

    Image* operator*() const { return m_image; }
    Image* operator->() const { return m_image; }
    operator Image*() { return m_image; }
    operator const Image*() const { return m_image; }
};

class PNGImage : public Image {
    std::string m_filename;
    TextureType m_type;

public:
    PNGImage(std::string filename, TextureType type, Private);

    static ImageRef create(Instance& instance, std::string filename, TextureType type = TextureType::image);

protected:
    void render(Instance& instance) override;
};

class IFont;

class TextImage : public Image {
    std::reference_wrapper<IFont> m_font;
    std::string m_text;

public:
    TextImage(IFont& font, std::string text, Private);

    static ImageRef create(Instance& instance, IFont& font, std::string text);

protected:
    void render(Instance& instance) override;
};

class BufferImage : public Image {
    USize m_size;
    TextureType m_type;
    std::unique_ptr<std::uint8_t[]> m_data;

public:
    BufferImage(USize size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data, Private);

    static ImageRef create(Instance& instance, USize size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data);

    void replace(std::unique_ptr<std::uint8_t[]>&& data);
    void compose(ImageData& picture, ICoords origin);

protected:
    void render(Instance& instance) override;
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
