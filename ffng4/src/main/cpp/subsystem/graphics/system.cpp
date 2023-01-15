#include "subsystem/graphics.h"
#include "system.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_graphics(instance.graphics()),
        m_display(std::make_shared<ogl::Display>(reinterpret_cast<NativeWindowType>(instance.window()))),
        m_fullscreenTarget(*m_display),
        m_blurTargets{ref(), ref()},
        m_offscreenTarget(ref()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance),
        m_windowSize(640, 480)
{ }

void GraphicsSystem::setWindowSize(unsigned int width, unsigned int height, FCoords reserve) {
    m_windowSize = {width, height};
    m_fullscreenTarget.setReserve(reserve);
    m_blurTargets[0].resizeBoth(width, height);
    m_blurTargets[1].resizeBoth(width, height);
    auto pixelSize = m_graphics.coords(Graphics::CoordSystems::window).in2out_dim({width, height});
    m_offscreenTarget.resize(pixelSize.x(), pixelSize.y(), (float)width, (float)height);
}

void GraphicsSystem::notifyDisplayResize() {
    m_fullscreenTarget.recalc();
    auto pixelSize = m_graphics.coords(Graphics::CoordSystems::window).in2out_dim(m_windowSize);
    m_offscreenTarget.resizeTexture(pixelSize.x(), pixelSize.y());
}
