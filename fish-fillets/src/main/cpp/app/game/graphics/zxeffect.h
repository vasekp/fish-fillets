#ifndef FISH_FILLETS_ZXEFFECT_H
#define FISH_FILLETS_ZXEFFECT_H

#include "common.h"
#include "instance.h"

class Image;
class DrawTarget;

class ZXEffect {
public:
    ZXEffect(Instance& m_instance);

    void render(DrawTarget& target, const Image* image);
    void update(int tickCount);

private:
    Instance& m_instance;
    float m_stripHeight;
    float m_stripOffset;
    int m_colors;
    int m_lastTick;
};

#endif //FISH_FILLETS_ZXEFFECT_H
