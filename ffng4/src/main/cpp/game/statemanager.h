#ifndef FISH_FILLETS_STATEMANAGER_H
#define FISH_FILLETS_STATEMANAGER_H

#include "instance.h"
#include "screen.h"

enum class GameState {
    WorldMap,
    TestScreen
};

class StateManager {
    Instance* m_instance;
    std::vector<std::unique_ptr<GameScreen>> m_screens;
    GameState m_state;

public:
    StateManager(Instance* instance);

    GameState getState() const { return m_state; }
    void setState(GameState state);
    GameScreen* curScreen() { return m_screens.back().get(); }

};

#endif //FISH_FILLETS_STATEMANAGER_H