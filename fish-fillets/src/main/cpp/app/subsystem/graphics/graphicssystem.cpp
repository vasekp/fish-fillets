#include "subsystem/graphics.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_graphics(instance.graphics()),
        m_display(std::make_shared<ogl::Display>(reinterpret_cast<NativeWindowType>(instance.window()))),
        m_fullscreenTarget(*this, *m_display),
        m_blurTargets{*this, *this},
        m_offscreenTarget(*this),
        m_shaders(instance, *this)
{
    resizeBuffers();
}

void GraphicsSystem::resizeBuffers() {
    auto size = m_display->size();
    m_offscreenTarget.resize(size.x(), size.y());
    auto scale = 1 / m_graphics.coords(Graphics::CoordSystems::base).scale;
    m_blurTargets[0].resize(size.x(), size.y(), scale);
    m_blurTargets[1].resize(size.x(), size.y(), scale);
}

void GraphicsSystem::bind(DrawTarget* target) {
    if(target != m_curTarget)
        target->bind();
}

void GraphicsSystem::present(TextureTarget& target) {
    m_fullscreenTarget.draw(m_offscreenTarget.texture(), m_shaders.copy(), m_graphics.coords(Graphics::CoordSystems::null));
    m_display->swap();
}
