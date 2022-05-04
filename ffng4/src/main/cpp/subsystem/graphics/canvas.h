#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Coords {
    int m_x;
    int m_y;
    float m_fx;
    float m_fy;

public:
    Coords(unsigned x, unsigned y) :
            m_x(x), m_y(y),
            m_fx(x), m_fy(y)
    { }

    Coords(int x, int y) :
            m_x(x), m_y(y),
            m_fx(x), m_fy(y)
    { }

    Coords(float fx, float fy) :
            m_x(fx), m_y(fy),
            m_fx(fx), m_fy(fy)
    { }

    Coords() : Coords(0, 0) { }

    int x() const { return m_x; }
    int y() const { return m_y; }
    float fx() const { return m_fx; }
    float fy() const { return m_fy; }
};

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

    std::pair<int, int> screen2canvas(int x, int y);
    std::pair<int, int> canvas2screen(int x, int y);
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H
