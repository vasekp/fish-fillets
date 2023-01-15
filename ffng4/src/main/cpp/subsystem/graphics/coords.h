#ifndef FISH_FILLETS_COORDS_H
#define FISH_FILLETS_COORDS_H

struct Coords {
    FCoords origin = {};
    float scale = 1.f;

    FCoords in2out(FCoords in) const { return scale * in + origin; }
    FCoords out2in(FCoords out) const { return (out - origin) / scale; }
    FCoords in2out_dim(FCoords in) const { return scale * in; }
    FCoords out2in_dim(FCoords out) const { return out / scale; }
};

#endif //FISH_FILLETS_COORDS_H