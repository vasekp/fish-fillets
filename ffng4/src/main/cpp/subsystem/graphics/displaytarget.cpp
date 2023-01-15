#include "subsystem/graphics.h"
#include "displaytarget.h"

DisplayTarget::DisplayTarget(const ogl::Display &display) :
    m_display(display),
    m_windowDim(display.width(), display.height()),
    m_viewport{{0.f, 0.f}, {m_display.width(), m_display.height()}}
{ }

void DisplayTarget::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_viewport.origin.x(), m_display.height() - m_viewport.origin.y() - m_viewport.extent.y(),
            m_viewport.extent.x(), m_viewport.extent.y());
}

void DisplayTarget::setReserve(FCoords reserve) {
    m_reserve = reserve;
}

void DisplayTarget::recalc() {
    Log::debug("resize fullscreen");
    m_windowDim = {m_display.width(), m_display.height()};
    m_viewport = {{0.f, 0.f}, {m_display.width(),m_display.height()}};
}