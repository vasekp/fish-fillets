#include "shape.h"

Shape::Shape(const std::string &string) :
    m_width(0),
    m_height(0)
{
    int x = 0;
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
            if(m_width == 0)
                m_width = x;
            else if(x != m_width)
                Log::error("Inhomogeneous shape");
            m_bits.push_back(line);
            line.reset();
            x = 0;
            break;
        default:
            Log::error("Wrong shape character: ", c);
        }
    if(line.any())
        Log::error("Last shape line unterminated, ignoring");

    m_height = (int)m_bits.size();
}

bool lineIntersect(const std::bitset<Shape::maxSize>& a, const std::bitset<Shape::maxSize>& b, int dx) {
    if(dx >= 0)
        return (a & (b << dx)).any();
    else
        return (a & (b >> (-dx))).any();
}

bool Shape::intersects(const Shape& other, ICoords d) const {
    if(d.x > (int)width() || d.y > (int)height() || -d.x > (int)other.width() || -d.y > (int)other.height())
        return false;
    if(d.y >= 0) {
        for(int i = d.y; i < height(); i++) {
            if(i - d.y >= other.height())
                break;
            const auto& bits1 = m_bits[i];
            const auto& bits2 = other.m_bits[i - d.y];
            if(lineIntersect(bits1, bits2, d.x))
                return true;
        }
    } else {
        for(int i2 = -d.y; i2 < other.height(); i2++) {
            if(i2 + d.y >= height())
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
    if(xy.x < 0 || xy.y < 0 || xy.x >= (int)width() || xy.y >= (int)height())
        return false;
    else
        return m_bits[xy.y][xy.x];
}
