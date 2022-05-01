#ifndef FISH_FILLETS_TYPES_H
#define FISH_FILLETS_TYPES_H

struct Color {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t alpha;

    constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0xFF) :
        red(r),
        green(g),
        blue(b),
        alpha(a)
    { }

    constexpr Color(std::uint32_t rgba) :
        red(rgba >> 24),
        green((rgba >> 16) & 0xFF),
        blue((rgba >> 8) & 0xFF),
        alpha(rgba & 0xFF)
    { }

    constexpr std::uint32_t rgba() const {
        return (red << 24) + (green << 16) + (blue << 8) + alpha;
    }
};

#endif //FISH_FILLETS_TYPES_H
