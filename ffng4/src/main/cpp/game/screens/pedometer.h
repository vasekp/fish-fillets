#ifndef FISH_FILLETS_GAME_PEDOMETER_H
#define FISH_FILLETS_GAME_PEDOMETER_H

#include "subsystem/graphics.h"
#include "game/structure/levelrecord.h"

class Pedometer {
    Instance& m_instance;
    LevelRecord& m_record;
    PNGImage m_pmImage;
    PNGImage m_digImage;

    std::array<int, 5> m_digits;
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
        FCoords origin;
        static constexpr FCoords size{43, 44};
    } m_buttons[3];

public:
    Pedometer(Instance& instance, LevelRecord& level);

    void draw(const DrawTarget& target, float dt);
    Buttons findButton(FCoords coords);
    LevelRecord& record() { return m_record; }

private:
    static constexpr ICoords pos{193, 141};
    static constexpr FCoords digitArray{275, 177};
    static constexpr FCoords digitSize{19, 24};
    static constexpr float digitSpeed = 4.f; // digits per second
};

#endif //FISH_FILLETS_GAME_PEDOMETER_H
