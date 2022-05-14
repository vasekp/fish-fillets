#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;

    template<typename T>
    constexpr Color(T r, T g, T b) : r(r), g(g), b(b) { }

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

class FCoords {
    int m_x;
    int m_y;
    float m_fx;
    float m_fy;

public:
    template<typename T>
    FCoords(T x, T y) : m_x(x), m_y(y), m_fx(x), m_fy(y) { }

    FCoords() : FCoords(0, 0) { }

    int x() const { return m_x; }
    int y() const { return m_y; }
    float fx() const { return m_fx; }
    float fy() const { return m_fy; }
};

struct ICoords {
    int x;
    int y;

    static const ICoords up;
    static const ICoords down;
    static const ICoords left;
    static const ICoords right;

    friend ICoords operator+(ICoords a, ICoords b) { return {a.x + b.x, a.y + b.y}; }
    friend ICoords operator-(ICoords a, ICoords b) { return {a.x - b.x, a.y - b.y}; }
    friend ICoords& operator+=(ICoords& a, ICoords b) { a.x += b.x; a.y += b.y; return a; }
    friend ICoords& operator-=(ICoords& a, ICoords b) { a.x -= b.x; a.y -= b.y; return a; }
};

inline constexpr ICoords ICoords::up = {0, -1};
inline constexpr ICoords ICoords::down = {0, +1};
inline constexpr ICoords ICoords::left = {-1, 0};
inline constexpr ICoords ICoords::right = {+1, 0};

#endif //FISH_FILLETS_TYPES_H