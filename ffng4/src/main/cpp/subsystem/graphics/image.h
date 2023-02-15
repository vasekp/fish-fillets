#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
protected:
    std::reference_wrapper<Instance> m_instance;
    ogl::Texture m_texture;

    Image(Instance& instance) : m_instance(instance), m_texture() { }
    Image(Image&&) noexcept;
    Image& operator=(Image&&) noexcept;
    virtual ~Image() noexcept;
    void init(); // Must be called at the end of derived classes' constructors.

public:
    auto& texture() const { return m_texture; }
    auto width() const { return m_texture.width(); }
    auto height() const { return m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }
    virtual void render() = 0;

private:
    friend class Graphics;
};

class PNGImage : public Image {
    std::string m_filename;

public:
    PNGImage(Instance& instance, std::string filename);
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
    void render() override;
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
