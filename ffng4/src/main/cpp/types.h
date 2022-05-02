#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;

    constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) :
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

    std::unique_ptr<float[]> gl() const {
        auto ret = std::make_unique<float[]>(4);
        ret[0] = (float)r / 255.f;
        ret[1] = (float)g / 255.f;
        ret[2] = (float)b / 255.f;
        ret[3] = 1.f;
        return ret;
    }

    friend constexpr bool operator==(const Color& x, const Color& y) {
        return x.r == y.r && x.g == y.g && x.b == y.b;
    }
};

#endif //FISH_FILLETS_TYPES_H
