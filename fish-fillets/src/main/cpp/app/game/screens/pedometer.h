#ifndef FISH_FILLETS_GAME_PEDOMETER_H
#define FISH_FILLETS_GAME_PEDOMETER_H

#include "subsystem/graphics.h"
#include "game/structure/levelrecord.h"
#include "game/screens/liveclock.h"

class Pedometer {
    Instance& m_instance;
    LevelRecord& m_record;
    PNGImage m_pmImage;
    PNGImage m_digImage;

    std::array<int, 5> m_digits;
    LiveClock::time_point m_createTime;

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
    Pedometer(Instance& instance, LevelRecord& level, LiveClock::time_point time);

    void draw(DrawTarget& target, LiveClock::time_point time);
    Buttons findButton(FCoords coords);
    LevelRecord& record() { return m_record; }

private:
    static constexpr ICoords pos{193, 141};
    static constexpr FCoords digitArray{275, 177};
    static constexpr FCoords digitSize{19, 24};
    static constexpr std::chrono::steady_clock::duration digitTime = 250ms;
};

#endif //FISH_FILLETS_GAME_PEDOMETER_H
