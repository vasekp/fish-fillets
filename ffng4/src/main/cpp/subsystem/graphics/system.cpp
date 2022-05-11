#include "subsystem/graphics.h"

GraphicsSystem::GraphicsSystem(Instance& instance) :
    m_display(std::make_shared<ogl::Display>(instance.app()->window)),
    m_displayTarget(*m_display),
    m_readBuffer(m_display),
    m_shaders(m_display, instance)
{ }