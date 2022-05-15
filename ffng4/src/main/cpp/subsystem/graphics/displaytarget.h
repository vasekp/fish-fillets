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
    void setWindow(unsigned int width, unsigned int height);
    FCoords size() const override { return m_windowDim; }

    FCoords screen2window(FCoords screen) const;
    FCoords window2screen(FCoords window) const;

    FCoords windowSize() const { return m_windowDim; }
    FCoords pixelSize() const { return m_viewport.extent; }
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
