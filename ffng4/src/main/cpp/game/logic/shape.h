#ifndef FISH_FILLETS_SHAPE_H
#define FISH_FILLETS_SHAPE_H

#include "common.h"
#include <bitset>

class Shape {
public:
    constexpr static std::size_t maxSize = 60;

private:
    std::vector<std::bitset<maxSize>> m_bits;
    unsigned m_width;
    unsigned m_height;

public:
    Shape(const std::string& string);

    unsigned width() const { return m_width; }
    unsigned height() const { return m_height; }
    const auto& operator[](unsigned y) const { return m_bits[y]; }

    bool intersects(const Shape& other, ICoords d) const;
    bool covers(ICoords xy) const;
};

#endif //FISH_FILLETS_SHAPE_H