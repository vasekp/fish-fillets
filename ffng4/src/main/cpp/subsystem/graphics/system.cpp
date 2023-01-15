#include "subsystem/graphics.h"
#include "system.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
        m_display(std::make_shared<ogl::Display>(reinterpret_cast<NativeWindowType>(instance.window()))),
        m_fullscreenTarget(*m_display),
        m_blurTargets{ref(), ref()},
        m_offscreenTarget(ref()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance)
{ }

void GraphicsSystem::resizeBuffers() {
    m_offscreenTarget.resize(m_display->width(), m_display->height());
    /*m_blurTargets[0].resizeBoth(width, height);
    m_blurTargets[1].resizeBoth(width, height);*/ // TODO
}