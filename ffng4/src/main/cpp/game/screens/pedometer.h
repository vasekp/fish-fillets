#ifndef FISH_FILLETS_GAME_PEDOMETER_H
#define FISH_FILLETS_GAME_PEDOMETER_H

#include "subsystem/graphics.h"

class Pedometer {
    Instance& m_instance;
    PNGImage m_pmImage;

public:
    enum class Buttons {
        none,
        retry,
        replay,
        close
    };

private:
    struct Button {
        Buttons type;
        PNGImage image;
        ICoords origin;
        inline static constexpr ICoords size{43, 44};
    } m_buttons[3];

public:
    Pedometer(Instance& instance);

    void draw(const DrawTarget& target, FCoords hover);
    Buttons findButton(FCoords coords);

private:
    static constexpr ICoords pos{193, 141};
};

#endif //FISH_FILLETS_GAME_PEDOMETER_H
