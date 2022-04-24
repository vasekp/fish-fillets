#ifndef HEADER_FIELD_H
#define HEADER_FIELD_H

class V2;
class Cube;

#include "NoCopy.h"

#include <vector>
#include <memory>

/**
 * Two dimensional game field.
 */
class Field : public NoCopy {
    private:
        int m_w;
        int m_h;
        std::unique_ptr<Cube> m_border;
        std::vector<std::vector<Cube*>> m_marks;
    public:
        Field(int w, int h);

        int getW() const { return m_w; }
        int getH() const { return m_h; }

        Cube *getModel(const V2 &loc);
        void setModel(const V2 &loc, Cube *model, Cube *toOverride);
};

#endif
