#ifndef FISH_FILLETS_SHAPE_H
#define FISH_FILLETS_SHAPE_H

#include "common.h"
#include "displacement.h"
#include <bitset>

class Shape {
    std::vector<std::bitset<64>> m_bits;
    unsigned m_width;
    unsigned m_height;

public:
    Shape(const std::string& string);

    unsigned width() const { return m_width; }
    unsigned height() const { return m_height; }

    bool intersects(const Shape& other, Displacement d) const;
};

#endif //FISH_FILLETS_SHAPE_H