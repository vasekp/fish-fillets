#include "subsystem/graphics.h"
#include "drawtarget.h"

void DrawTarget::draw(const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape) {
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
    if(!params.srcSize)
        params.srcSize = image.size();
    if(!params.area)
        params.area = params.srcSize;
    params.image.emplace(image);
    draw(program, coords, params, shape);
}
