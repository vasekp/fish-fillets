#include "subsystem/graphics.h"
#include "drawtarget.h"

void DrawTarget::draw(const ogl::Program& program, const Coords& coords, Program::Params params) {
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

    float vertices[4][2] = {
        {0, 0},
        {0, params.area.fy()},
        {params.area.fx(), 0},
        {params.area.fx(), params.area.fy()}
    };
    glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &vertices[0][0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawTarget::draw(TextureView image, const ogl::Program& program, const Coords& coords, Program::Params params) {
    if(!params.srcSize)
        params.srcSize = image.size();
    if(!params.area)
        params.area = params.srcSize;
    image.texture().bind();
    draw(program, coords, params);
}

void DrawTarget::draw(const Program& program, const Coords& coords, Program::Params params) {
    if(!params.srcSize)
        params.srcSize = params.area;
    else if(!params.area)
        params.area = params.srcSize;
    params.dstSize = size();
    params.coords = coords;
    params.flipY = flipY();
    program.run(*this, params);
}

void DrawTarget::draw(TextureView image, const Program& program, const Coords& coords, Program::Params params) {
    if(!params.srcSize)
        params.srcSize = image.size();
    if(!params.area)
        params.area = params.srcSize;
    params.image.emplace(image);
    draw(program, coords, params);
}
