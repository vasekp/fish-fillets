#ifndef FISH_FILLETS_GAME_STATEMANAGER_H
#define FISH_FILLETS_GAME_STATEMANAGER_H

#include "instance.h"
#include "screen.h"
#include "levelrecord.h"

enum class GameState {
    WorldMap,
    TestScreen,
    Intro,
    Credits,
    Game
};

class StateManager {
    Instance& m_instance;
    std::vector<std::unique_ptr<GameScreen>> m_screens;
    GameState m_state;

public:
    StateManager(Instance& instance);

    void setState(GameState state);
    void startLevel(const LevelRecord& record);
    GameState getState() const { return m_state; }
    GameScreen& curScreen() { return *m_screens.back(); }

private:
    void playIntro();
};

#endif //FISH_FILLETS_GAME_STATEMANAGER_H