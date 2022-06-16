#include "subsystem/graphics.h"
#include "system.h"
#include "platform/instance.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_display(std::make_shared<ogl::Display>(instance.platform().window())),
        m_windowTarget(*m_display, false),
        m_fullscreenTarget(*m_display, true),
        m_blurTargets{ref(), ref()},
        m_offscreenTarget(ref()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance)
{ }

void GraphicsSystem::setWindowSize(unsigned int width, unsigned int height, FCoords reserve) {
    m_fullscreenTarget.setReserve(reserve);
    m_windowTarget.setReserve(reserve);
    m_windowTarget.setWindow(width, height);
    m_blurTargets[0].resizeBoth(width, height);
    m_blurTargets[1].resizeBoth(width, height);
    auto pixelSize = m_windowTarget.nativeSize();
    m_offscreenTarget.resize(pixelSize.x(), pixelSize.y(), (float)width, (float)height);
}

void GraphicsSystem::notifyDisplayResize() {
    m_fullscreenTarget.recalc();
    m_windowTarget.recalc();
    auto pixelSize = m_windowTarget.nativeSize();
    m_offscreenTarget.resizeTexture(pixelSize.x(), pixelSize.y());
}
