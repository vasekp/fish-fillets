#include "subsystem/graphics.h"

Image::~Image() {
    if(_ref.expired())
        _texture.invalidate();
}

void Image::reload(const Decoders& decoder, const std::shared_ptr<Graphics>& ref) {
    _texture.invalidate();
    _ref = {ref};
    _texture = decoder.loadImage(_filename);
}