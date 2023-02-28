#ifndef FISH_FILLETS_GAME_PEDOMETER_H
#define FISH_FILLETS_GAME_PEDOMETER_H

#include "subsystem/graphics.h"

class Pedometer {
    Instance& m_instance;
    PNGImage m_pmImage;
    PNGImage m_digImage;

    std::array<float, 5> m_digits;
    float m_time;

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
    Pedometer(Instance& instance, unsigned steps);

    void draw(const DrawTarget& target, float dt, FCoords hover);
    Buttons findButton(FCoords coords);

private:
    static constexpr ICoords pos{193, 141};
    static constexpr ICoords digitArray{275, 177};
    static constexpr ICoords digitSize{19, 24};
    static constexpr float digitSpeed = 4.f; // digits per second
};

#endif //FISH_FILLETS_GAME_PEDOMETER_H
