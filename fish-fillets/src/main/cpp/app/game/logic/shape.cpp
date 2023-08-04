#include "shape.h"

Shape::Shape(const std::string &string) :
    m_width(0),
    m_height(0)
{
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
            m_width = std::max(m_width, x);
            m_bits.push_back(line);
            line.reset();
            x = 0;
            break;
        default:
            Log::error("Wrong shape character: ", c);
        }
    if(line.any())
        Log::error("Last shape line unterminated, ignoring");

    m_height = (unsigned)m_bits.size();
}

bool lineIntersect(const std::bitset<Shape::maxSize>& a, const std::bitset<Shape::maxSize>& b, int dx) {
    if(dx >= 0)
        return (a & (b << dx)).any();
    else
        return (a & (b >> (-dx))).any();
}

bool Shape::intersects(const Shape& other, ICoords d) const {
    if(d.x > (int)m_width || d.y > (int)m_height || -d.x > (int)other.width() || -d.y > (int)other.height())
        return false;
    if(d.y >= 0) {
        for(auto i = (unsigned)d.y; i < m_height; i++) {
            if(i - d.y >= other.height())
                break;
            const auto& bits1 = m_bits[i];
            const auto& bits2 = other.m_bits[i - d.y];
            if(lineIntersect(bits1, bits2, d.x))
                return true;
        }
    } else {
        for(auto i2 = (unsigned)(-d.y); i2 < other.height(); i2++) {
            if(i2 + d.y >= m_height)
                break;
            const auto& bits1 = m_bits[i2 + d.y];
            const auto& bits2 = other.m_bits[i2];
            if(lineIntersect(bits1, bits2, d.x))
                return true;
        }
    }
    return false;
}

bool Shape::covers(ICoords xy) const {
    if(xy.x < 0 || xy.y < 0 || xy.x >= (int)m_width || xy.y >= (int)m_height)
        return false;
    else
        return m_bits[xy.y][xy.x];
}
