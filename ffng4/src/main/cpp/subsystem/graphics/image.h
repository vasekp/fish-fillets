#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class GraphicsSystem;

class Image {
    std::string m_filename;
    std::weak_ptr<const GraphicsSystem> m_ref{};
    ogl::Texture m_texture{};

public:
    Image(std::string filename) : m_filename(std::move(filename)) { }
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;
    ~Image();

    auto& texture() const { return m_texture; }
    auto width() const { return m_texture.width(); }
    auto height() const { return m_texture.height(); }

    void reload(Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H