#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

#include "drawtarget.h"

class DisplayTarget : public DrawTarget {
    const ogl::Display& m_display;
    FCoords m_origin;
    FCoords m_size;

public:
    DisplayTarget(const ogl::Display& display) : m_display(display) { }

    void bind() const override { m_display.bind(); }

    FCoords size() const override { return m_display.getViewport().second; }

protected:
    bool flipY() const override { return true; }
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H