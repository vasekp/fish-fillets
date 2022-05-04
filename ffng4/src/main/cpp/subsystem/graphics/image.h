#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class GraphicsSystem;

class Image {
    std::string m_filename;
    std::shared_ptr<ogl::Texture> m_texture;

public:
    Image(std::string filename) : m_filename(std::move(filename)), m_texture(std::make_shared<ogl::Texture>()) { }

    auto& texture() const { return *m_texture; }
    auto width() const { return m_texture->width(); }
    auto height() const { return m_texture->height(); }

    void reload(Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H