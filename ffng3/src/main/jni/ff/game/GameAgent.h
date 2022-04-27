#ifndef HEADER_GAMEAGENT_H
#define HEADER_GAMEAGENT_H

class StateManager;

#include "common.h"
#include "BaseAgent.h"
#include "Name.h"

/**
 * Create game.
 * GameAgent uses StateManager to manage
 * WorldMap, Pedometer, Level and DemoMode.
 */
class GameAgent : public BaseAgent {
    AGENT(GameAgent, Name::GAME_NAME);
    private:
        StateManager *m_manager;

protected:
        virtual void own_init();
        virtual void own_update();
        virtual void own_shutdown();
};

#endif
