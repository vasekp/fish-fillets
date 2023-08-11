#include "subsystem/graphics.h"
#include "graphicssystem.h"
#include "drawtarget.h"

void DrawTarget::drawMain(const BaseProgram& program,
        const Coords& coords,
        BaseProgram::Params params,
        BaseProgram::Shape shape,
        const BaseProgram::Textures& textures) {
    m_system.bind(this);
    if(!params.srcSize)
        params.srcSize = params.area;
    else if(!params.area)
        params.area = params.srcSize;
    params.dstSize = size();
    params.coords = {coords.origin.x, coords.origin.y, coords.scale, flipY() ? -1.f : 1.f};
    program.run(m_system, *this, params, shape, textures);
}

void DrawTarget::draw(const BaseProgram& program, const Coords& coords, BaseProgram::Params params) {
    drawMain(program, coords, params, BaseProgram::Shape::rect, {});
}

void DrawTarget::draw(const Image* image,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params) {
    draw({image->texture()}, program, coords, params);
}

void DrawTarget::draw(const Texture& texture,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params) {
    draw(BaseProgram::Textures{texture}, program, coords, params);
}

void DrawTarget::draw(BaseProgram::Textures textures,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params) {
    m_system.bind(this);
    if(!params.srcSize)
        params.srcSize = textures.begin()->get().logSize();
    if(!params.area)
        params.area = params.srcSize;
    drawMain(program, coords, params, BaseProgram::Shape::rect, textures);
}

void DrawTarget::draw(BaseProgram::Shape shape,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params) {
    drawMain(program, coords, params, shape, {});
}
