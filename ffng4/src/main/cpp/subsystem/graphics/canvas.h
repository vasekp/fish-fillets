#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Coords {
    unsigned m_x;
    unsigned m_y;
    float m_fx;
    float m_fy;

public:
    Coords(unsigned x, unsigned y) :
            m_x(x), m_y(y),
            m_fx(x), m_fy(y)
    { }

    unsigned x() const { return m_x; }
    unsigned y() const { return m_y; }
    float fx() const { return m_fx; }
    float fy() const { return m_fy; }
};

class Canvas {
    Coords m_displayDim;
    Coords m_windowDim;

    struct {
        int vx, vy, vw, vh;
    } m_viewport;

public:
    Canvas(unsigned displayWidth, unsigned displayHeight);

    void setWindowSize(unsigned width, unsigned height);
    void bind() const;

    void drawImage(const Image& image, const ogl::Program& program, GLuint destX = 0, GLuint destY = 0) const;

    std::pair<int, int> screen2canvas(int x, int y);
    std::pair<int, int> canvas2screen(int x, int y);
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H
