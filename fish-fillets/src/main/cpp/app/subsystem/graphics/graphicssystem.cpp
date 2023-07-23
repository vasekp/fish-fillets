#include "subsystem/graphics.h"

void GraphicsSystem::resizeBuffers() {
    auto size = m_display.size();
    m_offscreenTarget.resize(size.x, size.y);
    auto scale = 1 / m_graphics.coords(Graphics::CoordSystems::base).scale;
    m_blurTargets[0].resize(size.x, size.y, scale);
    m_blurTargets[1].resize(size.x, size.y, scale);
}

void GraphicsSystem::newFrame() {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    m_offscreenTarget.bind();
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void GraphicsSystem::bind(DrawTarget* target) {
    if(target != m_curTarget)
        target->bind();
}

void GraphicsSystem::setViewport(ICoords origin, ICoords size) {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    m_display.setViewport(origin, size);
#endif
}

void GraphicsSystem::setScissor(ICoords from, ICoords to) {
    ICoords size = to - from;
    glScissor(from.x, from.y, size.x, size.y);
    glEnable(GL_SCISSOR_TEST);
}

void GraphicsSystem::releaseScissor() {
    glDisable(GL_SCISSOR_TEST);
}

void GraphicsSystem::present(TextureTarget& target) {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    DisplayTarget{*this, m_display}.draw(m_offscreenTarget.texture(), m_shaders.copy(), m_graphics.coords(Graphics::CoordSystems::null));
    m_display.swap();
#endif
}
