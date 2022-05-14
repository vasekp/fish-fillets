#include "shape.h"

Shape::Shape(const std::string &string) :
    m_width(0),
    m_height(0)
{
    int x = 0;
    std::bitset<64> line;
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
                LOGE("Inhomogeneous shape");
            m_bits.push_back(line);
            line.reset();
            x = 0;
            break;
        default:
            LOGE("Wrong shape character: %c", c);
        }
    if(line.any())
        LOGE("Last shape line unterminated, ignoring");

    m_height = (int)m_bits.size();
}

bool lineIntersect(const std::bitset<64>& a, const std::bitset<64>& b, int dx) {
    if(dx >= 0)
        return (a & (b << dx)).any();
    else
        return (a & (b >> (-dx))).any();
}

bool Shape::intersects(const Shape& other, Displacement d) const {
    if(d.dx > width() || d.dy > height() || -d.dx > other.width() || -d.dy > other.height())
        return false;
    if(d.dy >= 0) {
        for(int i = d.dy; i < height(); i++) {
            if(i - d.dy >= other.height())
                break;
            const auto& bits1 = m_bits[i];
            const auto& bits2 = other.m_bits[i - d.dy];
            if(lineIntersect(bits1, bits2, d.dx))
                return true;
        }
    } else {
        for(int i2 = -d.dy; i2 < other.height(); i2++) {
            if(i2 + d.dy >= height())
                break;
            const auto& bits1 = m_bits[i2 + d.dy];
            const auto& bits2 = other.m_bits[i2];
            if(lineIntersect(bits1, bits2, d.dx))
                return true;
        }
    }
    return false;
}