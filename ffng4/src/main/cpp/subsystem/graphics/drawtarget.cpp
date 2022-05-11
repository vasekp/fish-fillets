#include "subsystem/graphics.h"
#include "drawtarget.h"

void DrawTarget::setSize(unsigned int width, unsigned int height) {
    m_size = {width, height};
    resize(width, height);
}

void DrawTarget::blit(const ogl::Texture &texture, const ogl::Program &program, int destX, int destY,
                      int srcX, int srcY, GLuint width, GLuint height) const {
    glUseProgram(program);
    texture.bind();
    glUniform2f(program.uniform("uSrcSize"), (float) texture.width(), (float) texture.height());
    glUniform2f(program.uniform("uDstSize"), m_size.fx(), m_size.fy());
    glUniform2f(program.uniform("uSrcOffset"), (float) srcX, (float) srcY);
    glUniform2f(program.uniform("uDstOffset"), (float) destX, (float) destY);
    GraphicsUtils::rect(0u, 0u, width == fullSize ? texture.width() : width, height == fullSize ? texture.height() : height);
}