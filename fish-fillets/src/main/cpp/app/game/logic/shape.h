#ifndef FISH_FILLETS_SHAPE_H
#define FISH_FILLETS_SHAPE_H

#include "common.h"

class Shape {
public:
    Shape(const std::string& string);

    USize size() const { return m_size; }
    const auto& operator[](unsigned y) const { return m_bits[y]; }

    bool intersects(const Shape& other, ICoords delta) const;
    bool covers(ICoords xy) const;

    static constexpr unsigned maxSize = 60;

private:
    std::vector<std::bitset<maxSize>> m_bits;
    USize m_size;
};

#endif //FISH_FILLETS_SHAPE_H
