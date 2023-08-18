#ifndef FISH_FILLETS_COORDS_H
#define FISH_FILLETS_COORDS_H

#include "common.h"

struct Coords {
    FCoords origin = {};
    float scale = 1.f;
    FCoords size = {};
    FCoords principal = {};

    auto in2out(FCoords in) const { return scale * in + origin; }
    auto out2in(FCoords out) const { return (out - origin) / scale; }
    auto in2out_dim(FCoords in) const { return scale * in; }
    auto in2out_dim(float in) const { return scale * in; }
    auto out2in_dim(FCoords out) const { return out / scale; }
    auto out2in_dim(float out) const { return out / scale; }
    FCoords pixelAlign(FCoords in) const { return out2in(in2out(in).round()); }

    Coords shifted(FCoords shift) const { return {origin + scale * shift, scale}; }
};

#endif //FISH_FILLETS_COORDS_H
