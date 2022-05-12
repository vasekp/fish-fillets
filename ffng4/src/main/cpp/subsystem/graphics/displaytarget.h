#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

#include "drawtarget.h"

class DisplayTarget : public DrawTarget {
    const ogl::Display& m_display;
    Coords m_windowDim;
    struct {
        Coords origin;
        Coords extent;
    } m_viewport;

public:
    DisplayTarget(const ogl::Display& display) : m_display(display) { }

    void bind() const override;

    Coords screen2canvas(Coords screen) const;
    Coords canvas2screen(Coords canvas) const;
    Coords pixelSize() const;

protected:
    void resize(unsigned int width, unsigned int height) override;
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
