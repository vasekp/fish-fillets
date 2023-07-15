#include "subsystem/graphics.h"
#include "drawtarget.h"

void DrawTarget::clear() {
    m_system.bind(this);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DrawTarget::draw(const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape) {
    m_system.bind(this);
    if(!params.srcSize)
        params.srcSize = params.area;
    else if(!params.area)
        params.area = params.srcSize;
    params.dstSize = size();
    params.coords = coords;
    params.flipY = flipY();
    program.run(*this, params, shape);
}

void DrawTarget::draw(TextureView image, const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape) {
    m_system.bind(this);
    if(!params.srcSize)
        params.srcSize = image.size();
    if(!params.area)
        params.area = params.srcSize;
    params.image.emplace(image);
    draw(program, coords, params, shape);
}

void DrawTarget::setScissor(FCoords from, FCoords to) {
    FCoords size = to - from;
    glScissor(from.x(), from.y(), size.x(), size.y());
    glEnable(GL_SCISSOR_TEST);
}

void DrawTarget::releaseScissor() {
    glDisable(GL_SCISSOR_TEST);
}
