#include "subsystem/graphics.h"
#include "system.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_display(std::make_shared<ogl::Display>(instance.app()->window)),
        m_windowTarget(*m_display),
        m_fullscreenTarget(*m_display),
        m_blurTargets{ref(), ref()},
        m_offscreenTarget(ref()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance)
{ }

void GraphicsSystem::setWindowSize(unsigned int width, unsigned int height, FCoords reserve) {
    m_fullscreenTarget.setReserve(reserve);
    m_windowTarget.setReserve(reserve);
    m_windowTarget.setWindow(width, height);
    m_blurTargets[0].resize(width, height);
    m_blurTargets[1].resize(width, height);
    auto pixelSize = m_windowTarget.nativeSize();
    m_offscreenTarget.resize(pixelSize.x(), pixelSize.y(), (float)width, (float)height);
}