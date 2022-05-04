#include "subsystem/graphics.h"

Canvas::Canvas(unsigned displayWidth, unsigned displayHeight) :
        m_displayDim(displayHeight, displayHeight),
        m_windowDim(0, 0),
        m_viewport()
{ }

void Canvas::setWindowSize(unsigned width, unsigned height) {
    m_windowDim = {width, height};
    float scale = std::min(m_displayDim.fx() / m_windowDim.fx(), m_displayDim.fy() / m_windowDim.fy());
    m_viewport = {(int)(m_displayDim.fx() - scale * m_windowDim.fx()) / 2,
                  (int)(m_displayDim.fy() - scale * m_windowDim.fy()) / 2,
                  (int)(scale * m_windowDim.fx()), (int)(scale * m_windowDim.fy())};
    LOGV("Viewport: %d %d %d %d (scale %f)", m_viewport.vx, m_viewport.vy, m_viewport.vw, m_viewport.vh, scale);
}

void Canvas::bind() const {
    glViewport(m_viewport.vx, m_viewport.vy, m_viewport.vw, m_viewport.vh);
}

void Canvas::drawImage(const Image& image, const ogl::Program& program, GLuint destX, GLuint destY) const {
    glUseProgram(program);
    image.texture().bind();
    glUniform2f(program.uniform("uSrcSize"), (float) image.width(), (float) image.height());
    glUniform2f(program.uniform("uDstSize"), m_windowDim.fx(), m_windowDim.fy());
    glUniform2f(program.uniform("uDstOffset"), (float)destX, (float)destY);
    GraphicsUtils::rect(0u, 0u, image.width(), image.height());
}

std::pair<int, int> Canvas::screen2canvas(int x, int y) {
    return {
            (int)((float) (x - m_viewport.vx) / (float) m_viewport.vw * m_windowDim.fx()),
            (int)((float) (y - m_viewport.vy) / (float) m_viewport.vh * m_windowDim.fy())
    };
}

std::pair<int, int> Canvas::canvas2screen(int x, int y) {
    return {
            (int)((float) x / m_windowDim.fx() * (float) m_viewport.vw) + m_viewport.vx,
            (int)((float) y / m_windowDim.fy() * (float) m_viewport.vh) + m_viewport.vy
    };
}