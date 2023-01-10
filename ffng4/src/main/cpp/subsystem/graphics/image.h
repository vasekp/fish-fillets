#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
    std::reference_wrapper<Graphics> m_graphics;
    std::string m_filename;
    ogl::Texture m_texture;

public:
    Image(Graphics& graphics, std::string filename);
    Image(Image&&);
    Image& operator=(Image&&);
    ~Image();

    auto filename() const { return m_filename; }
    auto& texture() const { return m_texture; }
    auto width() const { return m_texture.width(); }
    auto height() const { return m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }

private:
    void renderTexture();
    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
