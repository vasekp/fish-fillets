#include "shape.h"

Shape::Shape(const std::string &string) {
    unsigned width = 0;
    unsigned x = 0;
    std::bitset<maxSize> line;
    for(const char c : string)
        switch(c) {
        case '.':
            x++;
            break;
        case 'X':
            line[x++] = true;
            break;
        case '\n':
            width = std::max(width, x);
            m_bits.push_back(line);
            line.reset();
            x = 0;
            break;
        default:
            Log::error("Wrong shape character: ", c);
        }
    if(line.any())
        Log::error("Last shape line unterminated, ignoring");
    m_size = {width, (unsigned)m_bits.size()};
}

bool lineIntersect(const std::bitset<Shape::maxSize>& a, const std::bitset<Shape::maxSize>& b, int dx) {
    if(dx >= 0)
        return (a & (b << dx)).any();
    else
        return (a & (b >> (-dx))).any();
}

bool Shape::intersects(const Shape& other, ICoords delta) const {
    if(!delta.within(-ICoords{other.size(), 1}, ICoords{m_size, 1}))
        return false;
    if(delta.y >= 0) {
        for(auto y = (unsigned)delta.y; y < m_size.height; y++) {
            if(y - delta.y >= other.size().height)
                break;
            const auto& bits1 = m_bits[y];
            const auto& bits2 = other.m_bits[y - delta.y];
            if(lineIntersect(bits1, bits2, delta.x))
                return true;
        }
    } else {
        for(auto y = (unsigned)(-delta.y); y < other.size().height; y++) {
            if(y + delta.y >= m_size.height)
                break;
            const auto& bits1 = m_bits[y + delta.y];
            const auto& bits2 = other.m_bits[y];
            if(lineIntersect(bits1, bits2, delta.x))
                return true;
        }
    }
    return false;
}

bool Shape::covers(ICoords xy) const {
    if(!xy.within({}, ICoords{m_size, 1}))
        return false;
    else
        return m_bits[xy.y][xy.x];
}
