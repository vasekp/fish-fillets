#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;

    template<typename T>
    constexpr Color(T r, T g, T b) :
            r(r),
            g(g),
            b(b)
    { }

    constexpr Color(std::uint32_t rgb) :
            r(rgb >> 16),
            g((rgb >> 8) & 0xFF),
            b(rgb & 0xFF)
    { }

    constexpr std::uint32_t rgb() const {
        return (r << 16) + (g << 8) + b;
    }

    constexpr float rf() const { return (float)r / 255.f; }
    constexpr float rg() const { return (float)g / 255.f; }
    constexpr float rb() const { return (float)b / 255.f; }

    std::unique_ptr<float[]> gl() const {
        auto ret = std::make_unique<float[]>(4);
        std::tie(ret[0], ret[1], ret[2], ret[3]) = std::tuple{rf(), rg(), rb(), 1.f};
        return ret;
    }

    friend constexpr bool operator==(const Color& x, const Color& y) {
        return x.r == y.r && x.g == y.g && x.b == y.b;
    }

    static const Color white;
};

inline constexpr Color Color::white{255, 255, 255};

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

#endif //FISH_FILLETS_TYPES_H