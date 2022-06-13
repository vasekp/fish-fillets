#include "subsystem/graphics.h"
#include "system.h"
#include "platform/instance.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_display(std::make_shared<ogl::Display>(instance.platform().window())),
        m_windowTarget(*m_display),
        m_fullscreenTarget(*m_display),
        m_blurTargets{ref(), ref()},
        m_offscreenTarget(ref()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance),
        m_windowWidth(0),
        m_windowHeight(0),
        m_reserve{}
{ }

void GraphicsSystem::setWindowSize(unsigned int width, unsigned int height, FCoords reserve) {
    m_fullscreenTarget.setReserve(reserve);
    m_windowTarget.setReserve(reserve);
    m_windowTarget.setWindow(width, height);
    if(width != m_windowWidth || height != m_windowHeight) {
        m_blurTargets[0].resize(width, height);
        m_blurTargets[1].resize(width, height);
    }
    auto pixelSize = m_windowTarget.nativeSize();
    m_offscreenTarget.resize(pixelSize.x(), pixelSize.y(), (float)width, (float)height);
    m_windowWidth = width;
    m_windowHeight = height;
    m_reserve = reserve;
}

void GraphicsSystem::notifyDisplayResize() {
    setWindowSize(m_windowWidth, m_windowHeight, m_reserve);
}
