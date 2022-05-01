#include "subsystem/graphics.h"

Image::~Image() {
    if(_ref.expired())
        _texture.invalidate();
}

void Image::reload(Instance* instance) {
    _texture.invalidate();
    _ref = {instance->graphics};
    _texture = instance->graphics->loadImage(_filename);
}