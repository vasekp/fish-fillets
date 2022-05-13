#include "subsystem/graphics.h"
#include "system.h"


GraphicsSystem::GraphicsSystem(Instance& instance) :
    m_display(std::make_shared<ogl::Display>(instance.app()->window)),
    m_displayTarget(*m_display),
    m_fullscreenTarget(*m_display),
    m_offscreenTarget{ref(), ref()},
    m_readBuffer(m_display),
    m_shaders(m_display, instance)
{ }

void GraphicsSystem::setWindowSize(unsigned int width, unsigned int height) {
    m_displayTarget.setSize(width, height);
    for(auto& target : m_offscreenTarget)
        target.setSize(width, height);
}