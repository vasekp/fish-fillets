#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

#include "drawtarget.h"

class DisplayTarget : public DrawTarget {
    const ogl::Display& m_display;
    FCoords m_windowDim;
    struct {
        FCoords origin;
        FCoords extent;
    } m_viewport;

public:
    DisplayTarget(const ogl::Display& display);

    void bind() const override;

    FCoords screen2canvas(FCoords screen) const;
    FCoords canvas2screen(FCoords canvas) const;
    FCoords pixelSize() const;

protected:
    void resize(unsigned int width, unsigned int height) override;
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
