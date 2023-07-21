#include "subsystem/graphics.h"
#include "drawtarget.h"

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

void DrawTarget::draw(const Texture& texture, const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape) {
    m_system.bind(this);
    if(!params.srcSize)
        params.srcSize = texture.logSize();
    if(!params.area)
        params.area = params.srcSize;
    params.texture = &texture;
    draw(program, coords, params, shape);
}

void DrawTarget::draw(const Image* image, const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape) {
    draw(image->texture(), program, coords, params, shape);
}
