#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Canvas {
    Coords m_displayDim;
    Coords m_windowDim;
    struct {
        Coords origin;
        Coords extent;
    } m_viewport;

public:
    Canvas(unsigned displayWidth, unsigned displayHeight);

    void setWindowSize(unsigned width, unsigned height);
    void bind() const;

    void drawImage(const Image& image, const ogl::Program& program, GLuint destX = 0, GLuint destY = 0) const;

    Coords screen2canvas(Coords screen);
    Coords canvas2screen(Coords canvas);
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H