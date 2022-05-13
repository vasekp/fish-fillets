#ifndef FISH_FILLETS_DISPLACEMENT_H
#define FISH_FILLETS_DISPLACEMENT_H

struct Displacement {
    int dx;
    int dy;

    static const Displacement up;
    static const Displacement down;
    static const Displacement left;
    static const Displacement right;

    friend Displacement operator+(Displacement a, Displacement b) { return {a.dx + b.dx, a.dy + b.dy}; }
    friend Displacement operator-(Displacement a, Displacement b) { return {a.dx - b.dx, a.dy - b.dy}; }
};

inline constexpr Displacement Displacement::up = {0, -1};
inline constexpr Displacement Displacement::down = {0, +1};
inline constexpr Displacement Displacement::left = {-1, 0};
inline constexpr Displacement Displacement::right = {+1, 0};

#endif //FISH_FILLETS_DISPLACEMENT_H
