#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0xFF) :
            r(r),
            g(g),
            b(b),
            a(a)
    { }

    constexpr Color(std::uint32_t rgba) :
            r(rgba >> 24),
            g((rgba >> 16) & 0xFF),
            b((rgba >> 8) & 0xFF),
            a(rgba & 0xFF)
    { }

    constexpr std::uint32_t rgba() const {
        return (r << 24) + (g << 16) + (b << 8) + a;
    }

    std::unique_ptr<float[]> gl() const {
        auto ret = std::make_unique<float[]>(4);
        ret[0] = (float)r / 255.f;
        ret[1] = (float)g / 255.f;
        ret[2] = (float)b / 255.f;
        ret[3] = (float)a / 255.f;
        return ret;
    }

    friend constexpr bool operator==(const Color& x, const Color& y) {
        return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a;
    }
};

#endif //FISH_FILLETS_TYPES_H
