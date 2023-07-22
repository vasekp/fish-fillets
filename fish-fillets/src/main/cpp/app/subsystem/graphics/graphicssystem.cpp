#include "subsystem/graphics.h"

void GraphicsSystem::resizeBuffers() {
    auto size = m_display->size();
    m_offscreenTarget.resize(size.x(), size.y());
    auto scale = 1 / m_graphics.coords(Graphics::CoordSystems::base).scale;
    m_blurTargets[0].resize(size.x(), size.y(), scale);
    m_blurTargets[1].resize(size.x(), size.y(), scale);
}

void GraphicsSystem::newFrame() {
    m_offscreenTarget.bind();
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsSystem::bind(DrawTarget* target) {
    if(target != m_curTarget)
        target->bind();
}

void GraphicsSystem::setScissor(FCoords from, FCoords to) {
    FCoords size = to - from;
    glScissor(from.x(), from.y(), size.x(), size.y());
    glEnable(GL_SCISSOR_TEST);
}

void GraphicsSystem::releaseScissor() {
    glDisable(GL_SCISSOR_TEST);
}

void GraphicsSystem::present(TextureTarget& target) {
    DisplayTarget{*this, *m_display}.draw(m_offscreenTarget.texture(), m_shaders.copy(), m_graphics.coords(Graphics::CoordSystems::null));
    m_display->swap();
}
