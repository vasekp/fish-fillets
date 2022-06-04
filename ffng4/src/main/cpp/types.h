#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

#include <ostream>

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

    std::unique_ptr<float[]> gl(float alpha = 1.f) const {
        auto ret = std::make_unique<float[]>(4);
        std::tie(ret[0], ret[1], ret[2], ret[3]) = std::tuple{alpha * rf(), alpha * rg(), alpha * rb(), alpha};
        return ret;
    }

    friend constexpr bool operator==(const Color& x, const Color& y) {
        return x.r == y.r && x.g == y.g && x.b == y.b;
    }

    static const Color white;
};

inline constexpr Color Color::white{255, 255, 255};

struct ICoords {
    int x;
    int y;

    static const ICoords up;
    static const ICoords down;
    static const ICoords left;
    static const ICoords right;

    friend bool operator==(ICoords a, ICoords b) { return a.x == b.x && a.y == b.y; }
    friend ICoords operator+(ICoords a, ICoords b) { return {a.x + b.x, a.y + b.y}; }
    friend ICoords operator-(ICoords a, ICoords b) { return {a.x - b.x, a.y - b.y}; }
    friend ICoords& operator+=(ICoords& a, ICoords b) { a.x += b.x; a.y += b.y; return a; }
    friend ICoords& operator-=(ICoords& a, ICoords b) { a.x -= b.x; a.y -= b.y; return a; }

    operator bool() const { return x != 0 || y != 0; }
    operator std::pair<int, int>() const { return {x, y}; }

    friend std::ostream& operator<<(std::ostream& os, ICoords coords) { return os << "[" << coords.x << "," << coords.y << "]"; }
};

using Direction = ICoords;

inline constexpr Direction Direction::up = {0, -1};
inline constexpr Direction Direction::down = {0, +1};
inline constexpr Direction Direction::left = {-1, 0};
inline constexpr Direction Direction::right = {+1, 0};

class FCoords {
    int m_x;
    int m_y;
    float m_fx;
    float m_fy;

public:
    template<typename T>
    FCoords(T x, T y) : m_x(x), m_y(y), m_fx(x), m_fy(y) { }

    FCoords(ICoords ic) : FCoords(ic.x, ic.y) { }
    FCoords() : FCoords(0, 0) { }

    int x() const { return m_x; }
    int y() const { return m_y; }
    float fx() const { return m_fx; }
    float fy() const { return m_fy; }
    float norm2() const { return m_fx * m_fx + m_fy * m_fy; }
    float length() const { return std::sqrt(norm2()); }
    friend float length(FCoords coords) { return coords.length(); }
    float dot(const FCoords other) const { return m_fx * other.m_fx + m_fy * other.m_fy; }
    FCoords clamp(float maxAbs) const { return {std::clamp(m_fx, -maxAbs, maxAbs), std::clamp(m_fy, -maxAbs, maxAbs)}; }
    FCoords project(FCoords other) const { return dot(other) / other.norm2() * other; }
    bool within(FCoords from, FCoords to) const { return m_fx >= from.m_fx && m_fx <= to.m_fx && m_fy >= from.m_fy && m_fy <= to.m_fy; }
    ICoords round() const { return {(int)(m_fx + .5f), (int)(m_fy + .5f)}; }
    friend ICoords round(FCoords coords) { return coords.round(); }

    friend FCoords operator+(FCoords a, FCoords b) { return {a.m_fx + b.m_fx, a.m_fy + b.m_fy}; }
    friend FCoords operator-(FCoords a, FCoords b) { return {a.m_fx - b.m_fx, a.m_fy - b.m_fy}; }
    friend FCoords& operator+=(FCoords& a, FCoords b) { a.m_fx += b.m_fx; a.m_fy += b.m_fy; return a; }
    friend FCoords& operator-=(FCoords& a, FCoords b) { a.m_fx -= b.m_fx; a.m_fy -= b.m_fy; return a; }
    friend FCoords operator*(float f, FCoords c) { return {f * c.fx(), f * c.fy()}; }
    friend FCoords operator*(FCoords c, float f) { return {f * c.fx(), f * c.fy()}; }
    friend FCoords operator/(FCoords c, float f) { return {c.fx() / f, c.fy() / f}; }

    friend bool operator==(FCoords a, FCoords b) { return a.m_fx == b.m_fx && a.m_fy == b.m_fy; }
    friend bool operator>(FCoords a, FCoords b) { return std::abs(a.m_fx) > std::abs(b.m_fx) || std::abs(a.m_fy) > std::abs(b.m_fy); }
    friend bool operator>=(FCoords a, FCoords b) { return a > b || a == b; }
    friend bool operator||(FCoords a, FCoords b) {
        return (a.m_fx == 0 && b.m_fx == 0 && a.m_fy * b.m_fy > 0) ||
                (a.m_fy == 0 && b.m_fy == 0 && a.m_fx * b.m_fx > 0);
    }

    friend std::ostream& operator<<(std::ostream& os, FCoords coords) { return os << "[" << coords.m_fx << "," << coords.m_fy << "]"; }
};

#endif //FISH_FILLETS_TYPES_H