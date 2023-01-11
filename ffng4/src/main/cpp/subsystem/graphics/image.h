#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
protected:
    std::reference_wrapper<Graphics> m_graphics;
    ogl::Texture m_texture;

    Image(Graphics& graphics) : m_graphics(graphics), m_texture() { }
    Image(Image&&) noexcept;
    Image& operator=(Image&&) noexcept;
    virtual ~Image() noexcept;
    void init(); // Must be called at the end of derived classes' constructors.

public:
    auto& texture() const { return m_texture; }
    auto width() const { return m_texture.width(); }
    auto height() const { return m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }

private:
    virtual void renderTexture() = 0;
    friend class Graphics;
};

class PNGImage : public Image {
    std::string m_filename;

public:
    PNGImage(Graphics& graphics, std::string filename);

    auto filename() const { return m_filename; }

private:
    void renderTexture() override;
};

class TextImage : public Image {
    std::reference_wrapper<Font> m_font;
    std::string m_text;

public:
    TextImage(Graphics& graphics, Font& font, std::string text);

private:
    void renderTexture() override;
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
