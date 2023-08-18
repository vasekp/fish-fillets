#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Texture;

class Image {
protected:
    Image() = default;
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

public:
    virtual ~Image();

    const Texture& texture() const;
    FCoords size() const;

    virtual void render(Instance& instance) = 0;

protected:
    std::unique_ptr<Texture> m_texture;

    enum class Private { tag };
};

class ImageRef {
public:
    ImageRef(Instance& instance, const std::unique_ptr<Image>& ptr);
    ImageRef(const ImageRef&) = delete;
    ImageRef& operator=(const ImageRef&) = delete;
    ImageRef(ImageRef&&) noexcept;
    ImageRef& operator=(ImageRef&&) noexcept;
    ~ImageRef();

    Image* operator*() const { return m_image; }
    Image* operator->() const { return m_image; }
    operator Image*() { return m_image; }
    operator const Image*() const { return m_image; }

private:
    std::reference_wrapper<Instance> m_instance;
    Image* m_image;
};

class PNGImage : public Image {
public:
    PNGImage(std::string filename, TextureType type, Private);

    static ImageRef create(Instance& instance, std::string filename, TextureType type = TextureType::image);

    void render(Instance& instance) override;

private:
    std::string m_filename;
    TextureType m_type;
};

class IFont;

class TextImage : public Image {
public:
    TextImage(IFont& font, std::string text, Private);
    ~TextImage();

    static ImageRef create(Instance& instance, IFont& font, std::string text);

    void render(Instance& instance) override;

private:
    IFont& m_font;
    std::string m_text;
};

class BufferImage : public Image {
public:
    BufferImage(USize size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data, Private);

    static ImageRef create(Instance& instance, USize size, TextureType type, std::unique_ptr<std::uint8_t[]>&& data);

    void replace(std::unique_ptr<std::uint8_t[]>&& data);
    void compose(ImageData& picture, ICoords origin);

    void render(Instance& instance) override;

private:
    USize m_size;
    TextureType m_type;
    std::unique_ptr<std::uint8_t[]> m_data;
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
