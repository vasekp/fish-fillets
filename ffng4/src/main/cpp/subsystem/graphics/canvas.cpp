#include "subsystem/graphics.h"

Canvas::Canvas() :
        m_framebuffer(texWidth, texHeight),
        m_width(0), m_height(0)
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
    glUniform2f(program.uniform("uSrcSize"), (float) image->width(), (float) image->height());
    glUniform2f(program.uniform("uDstSize"), (float) m_width, (float) m_height);
    glUniform2f(program.uniform("uDstOffset"), (float)destX, (float)destY);
    GraphicsUtils::rect(0u, 0u, image->width(), image->height());
}

Color Canvas::getPixel(unsigned x, unsigned y) const {
    std::uint8_t pixels[4];
    bind();
    glReadPixels((GLint)x, (GLint)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return {pixels[0], pixels[1], pixels[2], pixels[3]};
}