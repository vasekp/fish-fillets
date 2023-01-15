#include "subsystem/graphics.h"
#include "system.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_graphics(instance.graphics()),
        m_display(std::make_shared<ogl::Display>(reinterpret_cast<NativeWindowType>(instance.window()))),
        m_fullscreenTarget(*m_display),
        m_blurTargets{ref(), ref()},
        m_offscreenTarget(ref()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance)
{ }

void GraphicsSystem::resizeBuffers() {
    auto width = m_display->width();
    auto height = m_display->height();
    m_offscreenTarget.resize(width, height);
    auto blurSize = m_graphics.coords(Graphics::CoordSystems::base).in2out_dim({width, height});
    m_blurTargets[0].resize(blurSize.x(), blurSize.y());
    m_blurTargets[1].resize(blurSize.x(), blurSize.y());
}