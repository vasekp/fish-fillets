#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

#include <ostream>
#include <charconv>

struct alignas(8) vec2 {
    std::array<float, 2> array;

    bool operator!() const { return array[0] == 0 && array[1] == 0; }

    operator const std::array<float, 2>&() const { return array; }
    const float* data() const { return array.data(); }
};

struct alignas(16) vec4 {
    std::array<float, 4> array;

    operator const std::array<float, 4>&() const { return array; }
    const float* data() const { return array.data(); }
};

struct ICoords {
    int x;
    int y;

    static const ICoords up;
    static const ICoords down;
    static const ICoords left;
    static const ICoords right;
    static const ICoords none;

    friend bool operator==(ICoords a, ICoords b) { return a.x == b.x && a.y == b.y; }
    friend ICoords operator+(ICoords a, ICoords b) { return {a.x + b.x, a.y + b.y}; }
    friend ICoords operator-(ICoords a, ICoords b) { return {a.x - b.x, a.y - b.y}; }
    friend ICoords& operator+=(ICoords& a, ICoords b) { a.x += b.x; a.y += b.y; return a; }
    friend ICoords& operator-=(ICoords& a, ICoords b) { a.x -= b.x; a.y -= b.y; return a; }
    bool within(ICoords from, ICoords to) const { return x >= from.x && x <= to.x && y >= from.y && y <= to.y; }

    bool operator!() const { return x == 0 && y == 0; }
    explicit operator bool() const { return !!*this; }
    operator std::pair<int, int>() const { return {x, y}; }

    friend std::ostream& operator<<(std::ostream& os, ICoords coords) { return os << "[" << coords.x << "," << coords.y << "]"; }
};

namespace std {
    /* Specialization for std::map<ICoords, ...> */
    template<>
    struct less<ICoords> {
        bool operator() (const ICoords& a, const ICoords& b) const {
            return less<std::pair<int, int>>{}(a, b);
        }
    };
}

using Direction = ICoords;

inline constexpr Direction Direction::up = {0, -1};
inline constexpr Direction Direction::down = {0, +1};
inline constexpr Direction Direction::left = {-1, 0};
inline constexpr Direction Direction::right = {+1, 0};
inline constexpr Direction Direction::none = {0, 0};

struct FCoords {
    float x;
    float y;

    template<typename T>
    constexpr FCoords(T x_, T y_) : x(x_), y(y_) { }
    constexpr FCoords(ICoords ic) : FCoords{(float)ic.x, (float)ic.y} { }
    constexpr FCoords() : FCoords{0.f, 0.f} { }

    operator std::pair<float, float>() const { return {x, y}; }
    operator vec2() const { return {x, y}; }

    constexpr float norm2() const { return x * x + y * y; }
    constexpr float length() const { return std::sqrt(norm2()); }
    constexpr friend float length(FCoords coords) { return coords.length(); }
    constexpr float dot(const FCoords other) const { return x * other.x + y * other.y; }
    constexpr FCoords clamp(float maxAbs) const { return {std::clamp(x, -maxAbs, maxAbs), std::clamp(y, -maxAbs, maxAbs)}; }
    constexpr FCoords project(FCoords other) const { return dot(other) / other.norm2() * other; }
    constexpr bool within(FCoords from, FCoords to) const { return x >= from.x && x <= to.x && y >= from.y && y <= to.y; }
    constexpr ICoords round() const { return {(int)x, (int)y}; }
    constexpr friend ICoords round(FCoords coords) { return coords.round(); }

    friend constexpr FCoords operator+(FCoords a, FCoords b) { return {a.x + b.x, a.y + b.y}; }
    friend constexpr FCoords operator-(FCoords a, FCoords b) { return {a.x - b.x, a.y - b.y}; }
    friend FCoords& operator+=(FCoords& a, FCoords b) { return a = a + b; }
    friend FCoords& operator-=(FCoords& a, FCoords b) { return a = a - b; }
    friend constexpr FCoords operator*(float f, FCoords c) { return {f * c.x, f * c.y}; }
    friend constexpr FCoords operator*(FCoords c, float f) { return {f * c.x, f * c.y}; }
    friend constexpr FCoords operator/(FCoords c, float f) { return {c.x / f, c.y / f}; }

    friend constexpr bool operator==(FCoords a, FCoords b) { return a.x == b.x && a.y == b.y; }
    friend constexpr bool operator>(FCoords a, FCoords b) { return std::abs(a.x) > std::abs(b.x) || std::abs(a.y) > std::abs(b.y); }
    friend constexpr bool operator>=(FCoords a, FCoords b) { return a > b || a == b; }
    friend constexpr bool operator||(FCoords a, FCoords b) {
        return (a.x == 0 && b.x == 0 && a.y * b.y > 0) || (a.y == 0 && b.y == 0 && a.x * b.x > 0);
    }
    bool constexpr operator!() const { return x == 0 && y == 0; }
    explicit constexpr operator bool() const { return !!*this; }

    friend std::ostream& operator<<(std::ostream& os, FCoords coords) { return os << "[" << coords.x << "," << coords.y << "]"; }
};

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

    vec4 gl(float alpha = 1.f) const {
        return {alpha * rf(), alpha * rg(), alpha * rb(), alpha};
    }

    friend constexpr bool operator==(const Color& x, const Color& y) {
        return x.r == y.r && x.g == y.g && x.b == y.b;
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        std::array<char, 8> str{0, 0, 0, 0, 0, 0, 0, 0};
        std::to_chars(str.begin(), &str[7], 0x1000000 + color.rgb(), 16);
        return os << &str[1];
    }

    static const Color white;
};

inline constexpr Color Color::white{255, 255, 255};

#endif //FISH_FILLETS_TYPES_H
