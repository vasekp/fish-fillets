#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
    std::string m_filename;
    ogl::Texture m_texture;

public:
    Image(std::string filename);
    Image(std::string filename, Instance& instance);

    auto filename() const { return m_filename; }
    auto& texture() const { return m_texture; }
    auto width() const { return m_texture.width(); }
    auto height() const { return m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }

    void reload(Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H
