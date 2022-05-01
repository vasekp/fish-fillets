#ifndef FISH_FILLETS_GRAPHICS_IMAGE_H
#define FISH_FILLETS_GRAPHICS_IMAGE_H

class Graphics;

class Image {
    std::string _filename;
    std::weak_ptr<Graphics> _ref{};
    ogl::Texture _texture{};

public:
    Image(std::string filename) : _filename(std::move(filename)) { }
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;
    ~Image();

    auto& texture() const { return _texture; }
    auto width() const { return _texture.width(); }
    auto height() const { return _texture.height(); }

    void reload(Instance* instance);
};

#endif //FISH_FILLETS_GRAPHICS_IMAGE_H