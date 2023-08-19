#include "subsystem/graphics/graphicsbackend.h"
#include "displaytarget.h"

// no extra members needed for OpenGL
GraphicsBackend::Detail* GraphicsBackend::backendDetail() {
    return nullptr;
}

GraphicsBackend::~GraphicsBackend() = default;

void GraphicsBackend::newFrame() {
    m_offscreenTarget.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_curTarget = &m_offscreenTarget;
}

void GraphicsBackend::bind(DrawTarget* target) {
    if(target == m_curTarget)
        return;
    target->bind();
    m_curTarget = target;
}

void GraphicsBackend::setViewport(ICoords origin, ICoords size) {
    m_display.setViewport(origin, size);
}

void GraphicsBackend::setScissor(ICoords from, ICoords to) {
    ICoords size = to - from;
    glScissor(from.x, from.y, size.x, size.y);
    glEnable(GL_SCISSOR_TEST);
}

void GraphicsBackend::releaseScissor() {
    glDisable(GL_SCISSOR_TEST);
}

void GraphicsBackend::present(TextureTarget& target) {
    DisplayTarget{*this, m_display}.draw(m_offscreenTarget.texture(), m_shaders.copy(), m_graphics.coords(Graphics::CoordSystems::null));
    m_display.swap();
}
