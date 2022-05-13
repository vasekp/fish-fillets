#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class GraphicsSystem;

class Image {
    struct Impl {
        SystemFile m_file;
        ogl::Texture m_texture;

        Impl(const SystemFile& file) : m_file(file), m_texture() { }
    };

    std::shared_ptr<Impl> m_impl;

public:
    Image(const SystemFile& file);

    auto filename() const { return m_impl->m_file.getPath(); }
    auto& texture() const { return m_impl->m_texture; }
    auto width() const { return m_impl->m_texture.width(); }
    auto height() const { return m_impl->m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }

    void reload(Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H