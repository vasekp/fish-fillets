#include "subsystem/graphics.h"
#include "drawtarget.h"

void DrawTarget::blit(TextureView source, const Coords& coords, const ogl::Program &program, float destX, float destY,
                      float srcX, float srcY, unsigned width, unsigned height) const {
    glUseProgram(program);
    source.texture().bind();
    glUniform2f(program.uniform("uSrcSize"), source.fwidth(), source.fheight());
    glUniform2f(program.uniform("uDstSize"), size().fx(), size().fy());
    glUniform2f(program.uniform("uSrcOffset"), srcX, srcY);
    glUniform2f(program.uniform("uDstOffset"), destX, destY);
    glUniform3f(program.uniform("uCoords"), coords.origin.fx(), coords.origin.fy(), coords.scale);
    glUniform2f(program.uniform("uSigns"), 1.f, this->flipY() ? -1.f : 1.f);
    GraphicsUtils::rect(0u, 0u, width == fullSize ? source.width() : width, height == fullSize ? source.height() : height);
}

void DrawTarget::fill(const Coords& coords, const ogl::Program &program, float x1, float y1, float x2, float y2) const {
    glUseProgram(program);
    glUniform2f(program.uniform("uSrcSize"), x2 - x1, y2 - y1);
    glUniform2f(program.uniform("uDstSize"), size().fx(), size().fy());
    glUniform2f(program.uniform("uSrcOffset"), 0, 0);
    glUniform2f(program.uniform("uDstOffset"), x1, y1);
    glUniform3f(program.uniform("uCoords"), coords.origin.fx(), coords.origin.fy(), coords.scale);
    glUniform2f(program.uniform("uSigns"), 1.f, this->flipY() ? -1.f : 1.f);
    GraphicsUtils::rect(0.f, 0.f, x2 - x1, y2 - y1);
}