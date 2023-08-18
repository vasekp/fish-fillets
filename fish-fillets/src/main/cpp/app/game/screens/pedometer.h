#ifndef FISH_FILLETS_GAME_PEDOMETER_H
#define FISH_FILLETS_GAME_PEDOMETER_H

#include "common.h"
#include "instance.h"
#include "subsystem/graphics/image.h"
#include "game/structure/levelrecord.h"
#include "game/screens/liveclock.h"

class DrawTarget;

class Pedometer {
public:
    enum class Buttons {
        none,
        retry,
        replay,
        close
    };

    Pedometer(Instance& instance, LevelRecord& level, LiveClock::time_point time);

    void draw(DrawTarget& target, LiveClock::time_point time);
    Buttons findButton(FCoords coords);
    LevelRecord& record() { return m_record; }

private:
    Instance& m_instance;
    LevelRecord& m_record;
    ImageRef m_pmImage;
    ImageRef m_digImage;

    std::array<int, 5> m_digits;
    LiveClock::time_point m_createTime;

    struct Button {
        Buttons type;
        ImageRef image;
        FCoords origin;
        static constexpr FCoords size{43, 44};
    } m_buttons[3];
};

#endif //FISH_FILLETS_GAME_PEDOMETER_H
