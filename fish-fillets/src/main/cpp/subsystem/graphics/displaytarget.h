#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

#include "drawtarget.h"

class DisplayTarget : public DrawTarget {
    const ogl::Display& m_display;
    FCoords m_origin;
    FCoords m_size;

public:
    DisplayTarget(GraphicsSystem& system, const ogl::Display& display) :
        DrawTarget(system), m_display(display) { }

    FCoords size() const override { return m_display.getViewport().second; }

private:
    bool flipY() const override { return true; }
    void bind() override { m_display.bind(); }
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
