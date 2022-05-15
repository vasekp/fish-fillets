#include "subsystem/graphics.h"

void DrawTarget::blit(TextureView source, const ogl::Program &program, float destX, float destY,
                      float srcX, float srcY, unsigned width, unsigned height) const {
    blitFlip(source, program, false, false, destX, destY, srcX, srcY, width, height);
}

void DrawTarget::blitFlip(TextureView source, const ogl::Program &program, bool flipX, bool flipY,
                          float destX, float destY, float srcX, float srcY,
                          unsigned int width, unsigned int height) const {
    glUseProgram(program);
    source.texture().bind();
    glUniform2f(program.uniform("uSrcSize"), source.fwidth(), source.fheight());
    glUniform2f(program.uniform("uDstSize"), size().fx(), size().fy());
    glUniform2f(program.uniform("uSrcOffset"), srcX, srcY);
    glUniform2f(program.uniform("uDstOffset"), destX, destY);
    glUniform2f(program.uniform("uSigns"), flipX ? -1.f : 1.f, flipY ? -1.f : 1.f);
    GraphicsUtils::rect(0u, 0u, width == fullSize ? source.width() : width, height == fullSize ? source.height() : height);
}
