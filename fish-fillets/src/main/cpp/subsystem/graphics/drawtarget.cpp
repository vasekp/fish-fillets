#include "subsystem/graphics.h"
#include "drawtarget.h"

void DrawTarget::draw(const ogl::Program& program, const Coords& coords, Params params) {
    glUseProgram(program);
    if(!params.srcSize)
        params.srcSize = params.area;
    else if(!params.area)
        params.area = params.srcSize;
    glUniform2f(program.uniform("uSrcSize"), params.srcSize.fx(), params.srcSize.fy());
    glUniform2f(program.uniform("uDstSize"), size().fx(), size().fy());
    glUniform2f(program.uniform("uSrcOffset"), params.src.fx(), params.src.fy());
    glUniform2f(program.uniform("uDstOffset"), params.dest.fx(), params.dest.fy());
    glUniform3f(program.uniform("uCoords"), coords.origin.fx(), coords.origin.fy(), coords.scale);
    glUniform2f(program.uniform("uSigns"), 1.f, flipY() ? -1.f : 1.f);
    GraphicsUtils::rect(0.f, 0.f, params.area.fx(), params.area.fy()); // TODO move to DrawTarget
}

void DrawTarget::draw(TextureView image, const ogl::Program& program, const Coords& coords, Params params) {
    if(!params.srcSize)
        params.srcSize = image.size();
    if(!params.area)
        params.area = params.srcSize;
    image.texture().bind();
    draw(program, coords, params);
}
