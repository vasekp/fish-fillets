#ifndef FISH_FILLETS_GAME_STATEMANAGER_H
#define FISH_FILLETS_GAME_STATEMANAGER_H

#include "instance.h"
#include "screen.h"
#include "levelrecord.h"

class Screens {
    Instance& m_instance;
    std::unique_ptr<GameScreen> m_screen;

public:
    enum class Mode {
        WorldMap,
        Intro,
        Credits
    };

    Screens(Instance& instance) : m_instance(instance) { };

    void startMode(Mode mode);
    void startLevel(const LevelRecord& record);
    GameScreen& curScreen() { return *m_screen; }

private:
    void playIntro();
};

#endif //FISH_FILLETS_GAME_STATEMANAGER_H