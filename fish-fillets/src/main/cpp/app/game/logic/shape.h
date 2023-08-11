#ifndef FISH_FILLETS_SHAPE_H
#define FISH_FILLETS_SHAPE_H

#include "common.h"
#include <bitset>

class Shape {
public:
    constexpr static unsigned maxSize = 60;

private:
    std::vector<std::bitset<maxSize>> m_bits;
    USize m_size;

public:
    Shape(const std::string& string);

    USize size() const { return m_size; }
    const auto& operator[](unsigned y) const { return m_bits[y]; }

    bool intersects(const Shape& other, ICoords delta) const;
    bool covers(ICoords xy) const;
};

#endif //FISH_FILLETS_SHAPE_H
