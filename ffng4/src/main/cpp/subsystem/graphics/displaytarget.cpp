#include "subsystem/graphics.h"
#include "displaytarget.h"

DisplayTarget::DisplayTarget(const ogl::Display &display) : m_display(display) {
    m_viewport = {{0.f, 0.f}, {(float)m_display.width(), (float)m_display.height()}};
}

void DisplayTarget::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_viewport.origin.x(), m_viewport.origin.y(), m_viewport.extent.x(), m_viewport.extent.y());
}

void DisplayTarget::setWindow(unsigned int width, unsigned int height) {
    FCoords displayDim{m_display.width(), m_display.height()};
    m_windowDim = {width, height};
    float scale = std::min(displayDim.fx() / m_windowDim.fx(), displayDim.fy() / m_windowDim.fy());
    m_viewport = {{(displayDim.fx() - scale * m_windowDim.fx()) / 2.f,
                                             (displayDim.fy() - scale * m_windowDim.fy()) / 2.f},
                  {scale * m_windowDim.fx(), scale * m_windowDim.fy()}};
    LOGI("Viewport: %d %d %d %d (scale %f)", m_viewport.origin.x(), m_viewport.origin.y(), m_viewport.extent.x(), m_viewport.extent.y(), scale);
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