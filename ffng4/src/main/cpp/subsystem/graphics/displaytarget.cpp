#include "subsystem/graphics.h"
#include "displaytarget.h"

DisplayTarget::DisplayTarget(const ogl::Display &display) :
    m_display(display),
    m_windowDim(display.width(), display.height()),
    m_viewport{{0.f, 0.f}, {(float)m_display.width(), (float)m_display.height()}},
    m_scale(1.f)
{ }

void DisplayTarget::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_viewport.origin.x(), m_display.height() - m_viewport.origin.y() - m_viewport.extent.y(),
            m_viewport.extent.x(), m_viewport.extent.y());
}

void DisplayTarget::setReserve(FCoords reserve) {
    m_reserve = reserve;
}

void DisplayTarget::setWindow(unsigned int width, unsigned int height) {
    auto displayDim = reducedDisplaySize();
    m_windowDim = {width, height};
    m_scale = std::min(displayDim.fx() / m_windowDim.fx(), displayDim.fy() / m_windowDim.fy());
    m_viewport.origin = (displayDim - m_scale * m_windowDim) / 2.f + m_reserve;
    m_viewport.extent = m_scale * m_windowDim;
    Log::info("Viewport: ", m_viewport.origin, " ", m_viewport.extent, " (scale ", m_scale, ")");
}

void DisplayTarget::recalc() {
    setWindow(m_windowDim.x(), m_windowDim.y());
}

FCoords DisplayTarget::screen2window(FCoords screen) const {
    return {
            (screen.fx() - m_viewport.origin.fx()) / m_viewport.extent.fx() * m_windowDim.fx(),
            (screen.fy() - m_viewport.origin.fy()) / m_viewport.extent.fy() * m_windowDim.fy()
    };
}

FCoords DisplayTarget::window2screen(FCoords window) const {
    return {
            window.fx() / m_windowDim.fx() * m_viewport.extent.fx() + m_viewport.origin.fx(),
            window.fy() / m_windowDim.fy() * m_viewport.extent.fy() + m_viewport.origin.fy()
    };
}
