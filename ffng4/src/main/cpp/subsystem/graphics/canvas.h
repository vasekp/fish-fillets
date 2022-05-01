#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Canvas {
    ogl::Framebuffer m_framebuffer;
    unsigned m_width;
    unsigned m_height;

public:
    Canvas(unsigned maxWidth, unsigned maxHeight);

    auto width() const { return m_width; }
    auto height() const { return m_height; }
    auto texWidth() const { return m_framebuffer.width(); }
    auto texHeight() const { return m_framebuffer.height(); }

    void resize(unsigned width, unsigned height);
    void bind() const;

    void drawImage(const Image* image, const ogl::Program& program, GLuint destX = 0, GLuint destY = 0) const;
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H
