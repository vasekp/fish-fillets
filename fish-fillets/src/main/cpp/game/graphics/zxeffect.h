#ifndef FISH_FILLETS_ZXEFFECT_H
#define FISH_FILLETS_ZXEFFECT_H

#include "subsystem/graphics.h"

class ZXEffect {
    Instance& m_instance;
    float m_stripHeight;
    float m_stripOffset;
    int m_colors;
    int m_lastTick;

    constexpr static float height1 = 38.5;
    constexpr static float height2 = 3.4;
    constexpr static float roomHeight = 36 * 15.f;

    constexpr static std::array<std::pair<Color, Color>, 2> colors{{
        {0x800000, 0x008080},
        {0x000080, 0x808000}
    }};

public:
    ZXEffect(Instance& m_instance);

    void render(DrawTarget& target, const Image* image);
    void update(int tickCount);
};

#endif //FISH_FILLETS_ZXEFFECT_H
