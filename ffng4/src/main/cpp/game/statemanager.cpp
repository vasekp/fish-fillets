#include "statemanager.h"
#include "game/worldmap.h"
#include "game/testscreen.h"

StateManager::StateManager(Instance *instance) :
    m_instance(instance),
    m_screens(),
    m_state(GameState::WorldMap)
{
    m_screens.push_back(std::make_unique<WorldMap>(m_instance));
}

void StateManager::setState(GameState state) {
    if(state == getState())
        return;
    auto start = std::chrono::steady_clock::now();
    switch(state) {
        case GameState::WorldMap:
            m_screens.erase(m_screens.begin() + 1, m_screens.end());
            if(m_instance->live)
                curScreen()->start();
            break;
        case GameState::TestScreen:
            assert(m_screens.size() == 1);
            {
                auto screen = std::make_unique<TestScreen>(m_instance,
                                                           "images/start/prvni-p.png",
                                                           "images/start/prvni-w.png",
                                                           "music/rybky04.ogg");
                m_screens.push_back(std::move(screen));
            }
            if(m_instance->live)
                curScreen()->start();
            break;
        case GameState::Intro:
            playIntro();
            // Don't set m_state.
            return;
    }
    m_state = state;
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("setState duration = %f s", diff.count());
}

void StateManager::playIntro() {
    auto& jni = m_instance->jni;
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}