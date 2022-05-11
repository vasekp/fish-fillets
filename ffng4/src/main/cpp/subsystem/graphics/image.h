#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class GraphicsSystem;

class Image {
    struct Impl {
        std::string m_filename;
        ogl::Texture m_texture;

        Impl(const std::string& filename) : m_filename(filename), m_texture() { }
    };

    std::shared_ptr<Impl> m_impl;

public:
    Image(const std::string& filename);

    auto& filename() const { return m_impl->m_filename; }
    auto& texture() const { return m_impl->m_texture; }
    auto width() const { return m_impl->m_texture.width(); }
    auto height() const { return m_impl->m_texture.height(); }

    operator const ogl::Texture&() const { return texture(); }

    void reload(Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H