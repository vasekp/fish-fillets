#include "subsystem/graphics.h"
#include "offscreentarget.h"

OffscreenTarget::OffscreenTarget(const std::shared_ptr<ogl::Display> &ref, unsigned int fullWidth, unsigned int fullHeight) :
    m_ref(ref), m_displayDim(fullWidth, fullHeight), m_framebuffer(ref), m_texture()
{ }

void OffscreenTarget::bind() const {
    m_framebuffer.bind();
    glViewport(0, 0, (GLint)texture().width(), (GLint)texture().height());
}

void OffscreenTarget::resize(unsigned int width, unsigned int height) {
    FCoords windowDim = {width, height};
    float scale = std::min(m_displayDim.fx() / windowDim.fx(), m_displayDim.fy() / windowDim.fy());
    FCoords size = scale * windowDim;
    m_texture = ogl::Texture::empty(m_ref, size.x(), size.y());
    m_framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
}