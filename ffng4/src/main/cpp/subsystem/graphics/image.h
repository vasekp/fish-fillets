#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Image {
    SystemFile m_file;
    ogl::Texture m_texture;

public:
    Image(SystemFile file);
    Image(const SystemFile& file, Instance& instance);

    auto filename() const { return m_file.path(); }
    auto& texture() const { return m_texture; }
    auto width() const { return m_texture.width(); }
    auto height() const { return m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }

    void reload(Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H