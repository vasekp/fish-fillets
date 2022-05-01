#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Canvas {
    ogl::Framebuffer m_framebuffer;
    unsigned m_width;
    unsigned m_height;

public:
    Canvas();

    auto width() const { return m_width; }
    auto height() const { return m_height; }

    void resize(unsigned width, unsigned height);
    void bind() const;

    void drawImage(const Image* image, const ogl::Program& program, GLuint destX = 0, GLuint destY = 0) const;
    Color getPixel(unsigned x, unsigned y) const;

    static constexpr unsigned texWidth = 1000;
    static constexpr unsigned texHeight = 1000;
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H
