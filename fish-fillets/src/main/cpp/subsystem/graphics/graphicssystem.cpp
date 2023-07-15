#include "subsystem/graphics.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_graphics(instance.graphics()),
        m_display(std::make_shared<ogl::Display>(reinterpret_cast<NativeWindowType>(instance.window()))),
        m_fullscreenTarget(*this, *m_display),
        m_blurTargets{*this, *this},
        m_offscreenTarget(*this),
        m_shaders(m_display, instance)
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
