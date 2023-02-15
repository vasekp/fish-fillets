#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

#include "drawtarget.h"

class DisplayTarget : public DrawTarget {
    const ogl::Display& m_display;

public:
    DisplayTarget(const ogl::Display& display) : m_display(display) { }

    void bind() const override {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_display.width(), m_display.height());
    }

    FCoords size() const override { return {m_display.width(), m_display.height()}; }

protected:
    bool flipY() const override { return true; }
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H