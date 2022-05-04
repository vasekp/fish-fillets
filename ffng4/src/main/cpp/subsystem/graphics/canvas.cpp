#include "subsystem/graphics.h"

Canvas::Canvas(unsigned displayWidth, unsigned displayHeight) :
        m_displayDim(displayWidth, displayHeight),
        m_windowDim(),
        m_viewport()
{ }

void Canvas::setWindowSize(unsigned width, unsigned height) {
    m_windowDim = {width, height};
    float scale = std::min(m_displayDim.fx() / m_windowDim.fx(), m_displayDim.fy() / m_windowDim.fy());
    m_viewport = {{(m_displayDim.fx() - scale * m_windowDim.fx()) / 2,
                   (m_displayDim.fy() - scale * m_windowDim.fy()) / 2},
                  {scale * m_windowDim.fx(), scale * m_windowDim.fy()}};
    LOGV("Viewport: %d %d %d %d (scale %f)", m_viewport.origin.x(), m_viewport.origin.y(), m_viewport.extent.x(), m_viewport.extent.y(), scale);
}

void Canvas::bind() const {
    glViewport(m_viewport.origin.x(), m_viewport.origin.y(), m_viewport.extent.x(), m_viewport.extent.y());
}

void Canvas::drawImage(const Image& image, const ogl::Program& program, GLuint destX, GLuint destY) const {
    glUseProgram(program);
    image.texture().bind();
    glUniform2f(program.uniform("uSrcSize"), (float) image.width(), (float) image.height());
    glUniform2f(program.uniform("uDstSize"), m_windowDim.fx(), m_windowDim.fy());
    glUniform2f(program.uniform("uDstOffset"), (float)destX, (float)destY);
    GraphicsUtils::rect(0u, 0u, image.width(), image.height());
}

Coords Canvas::screen2canvas(Coords screen) {
    return {
        (screen.fx() - m_viewport.origin.fx()) / m_viewport.extent.fx() * m_windowDim.fx(),
        (screen.fy() - m_viewport.origin.fy()) / m_viewport.extent.fy() * m_windowDim.fy()
    };
}

Coords Canvas::canvas2screen(Coords canvas) {
    return {
        canvas.fx() / m_windowDim.fx() * m_viewport.extent.fx() + m_viewport.origin.fx(),
        canvas.fy() / m_windowDim.fy() * m_viewport.extent.fy() + m_viewport.origin.fy()
    };
}