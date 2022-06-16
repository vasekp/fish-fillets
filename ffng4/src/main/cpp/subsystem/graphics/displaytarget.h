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
    float m_scale;
    FCoords m_reserve;

public:
    DisplayTarget(const ogl::Display& display);

    void bind() const override;
    void setReserve(FCoords reserve); /* Does NOT call setWindow. */
    void setWindow(unsigned int width, unsigned int height);
    FCoords size() const override { return m_windowDim; }
    void recalc();

    FCoords screen2window(FCoords screen) const;
    FCoords window2screen(FCoords window) const;

    FCoords windowSize() const { return m_windowDim; }
    FCoords nativeSize() const { return m_viewport.extent; }
    FCoords displayOffset() const { return m_reserve; }
    FCoords reducedDisplaySize() const { return FCoords{m_display.width(), m_display.height()} - m_reserve; }
    float pixelScale() const { return m_scale; }

protected:
    bool flipY() const override { return true; }
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
