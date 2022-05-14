#include "subsystem/graphics.h"

void DrawTarget::setSize(unsigned int width, unsigned int height) {
    m_size = {width, height};
    resize(width, height);
}

void DrawTarget::blit(const ogl::Texture &texture, const ogl::Program &program, float destX, float destY,
                      float srcX, float srcY, unsigned width, unsigned height, unsigned dstWidth, unsigned dstHeight) const {
    blitFlip(texture, program, false, false, destX, destY, srcX, srcY, width, height, dstWidth, dstHeight);
}

void DrawTarget::blitFlip(const ogl::Texture &texture, const ogl::Program &program, bool flipX, bool flipY,
                          float destX, float destY, float srcX, float srcY,
                          unsigned int width, unsigned int height, unsigned dstWidth, unsigned dstHeight) const {
    glUseProgram(program);
    texture.bind();
    glUniform2f(program.uniform("uSrcSize"), (float) texture.width(), (float) texture.height());
    glUniform2f(program.uniform("uDstSize"), dstWidth == fullSize ? m_size.fx() : (float)dstWidth, dstHeight == fullSize ? m_size.fy() : (float)dstHeight);
    glUniform2f(program.uniform("uSrcOffset"), srcX, srcY);
    glUniform2f(program.uniform("uDstOffset"), destX, destY);
    glUniform2f(program.uniform("uSigns"), flipX ? -1.f : 1.f, flipY ? -1.f : 1.f);
    GraphicsUtils::rect(0u, 0u, width == fullSize ? texture.width() : width, height == fullSize ? texture.height() : height);
}
