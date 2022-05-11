#include "subsystem/graphics.h"
#include "system.h"


GraphicsSystem::GraphicsSystem(Instance& instance) :
    m_display(std::make_shared<ogl::Display>(instance.app()->window)),
    m_displayTarget(*m_display),
    m_offscreenTarget(ref()),
    m_readBuffer(m_display),
    m_shaders(m_display, instance)
{ }

void GraphicsSystem::setWindowSize(unsigned int width, unsigned int height) {
    m_displayTarget.setSize(width, height);
    m_offscreenTarget.setSize(width, height);
}
