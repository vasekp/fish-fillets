#include "subsystem/graphics.h"

void DisplayTarget::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_viewport.origin.x(), m_viewport.origin.y(), m_viewport.extent.x(), m_viewport.extent.y());
}

void DisplayTarget::resize(unsigned int width, unsigned int height) {
    Coords displayDim{m_display.width(), m_display.height()};
    m_windowDim = {width, height};
    float scale = std::min(displayDim.fx() / m_windowDim.fx(), displayDim.fy() / m_windowDim.fy());
    m_viewport = {{(displayDim.fx() - scale * m_windowDim.fx()) / 2.f,
                                             (displayDim.fy() - scale * m_windowDim.fy()) / 2.f},
                  {scale * m_windowDim.fx(), scale * m_windowDim.fy()}};
    LOGV("Viewport: %d %d %d %d (scale %f)", m_viewport.origin.x(), m_viewport.origin.y(), m_viewport.extent.x(), m_viewport.extent.y(), scale);
}

Coords DisplayTarget::screen2canvas(Coords screen) {
    return {
            (screen.fx() - m_viewport.origin.fx()) / m_viewport.extent.fx() * m_windowDim.fx(),
            (screen.fy() - m_viewport.origin.fy()) / m_viewport.extent.fy() * m_windowDim.fy()
    };
}

Coords DisplayTarget::canvas2screen(Coords canvas) {
    return {
            canvas.fx() / m_windowDim.fx() * m_viewport.extent.fx() + m_viewport.origin.fx(),
            canvas.fy() / m_windowDim.fy() * m_viewport.extent.fy() + m_viewport.origin.fy()
    };
}