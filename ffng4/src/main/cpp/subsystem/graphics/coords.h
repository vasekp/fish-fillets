#ifndef FISH_FILLETS_COORDS_H
#define FISH_FILLETS_COORDS_H

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

#endif //FISH_FILLETS_COORDS_H
