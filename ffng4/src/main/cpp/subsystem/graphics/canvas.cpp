#include "subsystem/graphics.h"

Canvas::Canvas(unsigned maxWidth, unsigned maxHeight) :
        m_framebuffer(maxWidth, maxHeight),
        m_width(0),
        m_height(0)
{
    glActiveTexture(Shaders::texCanvas_gl);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer.texture());
    glActiveTexture(Shaders::texImage_gl);
}

void Canvas::resize(unsigned width, unsigned height) {
    assert(width <= m_framebuffer.width() && height <= m_framebuffer.height());
    m_width = width;
    m_height = height;
}

void Canvas::bind() const {
    m_framebuffer.bind();
    glViewport(0, 0, (GLint)width(), (GLint)height());
}

void Canvas::drawImage(const Image* image, const ogl::Program& program, GLuint destX, GLuint destY) const {
    glUseProgram(program);
    image->texture().bind();
    glUniform1i(program.uniform("uTexture"), Shaders::texImage_shader);
    glUniform2f(program.uniform("uSrcSize"), (float) image->width(), (float) image->height());
    glUniform2f(program.uniform("uDstSize"), (float) m_width, (float) m_height);
    glUniform2f(program.uniform("uSrcOffset"), 0.f, 0.f);
    glUniform2f(program.uniform("uDstOffset"), (float)destX, (float)destY);
    GraphicsUtils::rect(0u, 0u, image->width(), image->height());
}