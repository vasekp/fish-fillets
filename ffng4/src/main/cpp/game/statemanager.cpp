#include "statemanager.h"
#include "game/worldmap.h"
#include "game/credits.h"
#include "game/levelscreen.h"

StateManager::StateManager(Instance& instance) :
    m_instance(instance),
    m_screens(),
    m_state(GameState::WorldMap)
{
    m_screens.push_back(std::make_unique<WorldMap>(m_instance));
}

void StateManager::setState(GameState state) {
    if(state == getState()) {
        LOGE("setState to current state");
        return;
    }
    auto start = std::chrono::steady_clock::now();
    switch(state) {
        case GameState::WorldMap:
            m_screens.erase(m_screens.begin() + 1, m_screens.end());
            if(m_instance.live)
                curScreen().start();
            break;
        case GameState::Credits:
            assert(m_screens.size() == 1);
            {
                auto screen = std::make_unique<CreditsScreen>(m_instance);
                m_screens.push_back(std::move(screen));
            }
            if(m_instance.live)
                curScreen().start();
            break;
        case GameState::Intro:
            playIntro();
            // Don't set m_state.
            return;
        default:
            throw std::logic_error("can't change to this state directly");
    }
    m_state = state;
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("setState duration = %f s", diff.count());
}

void StateManager::startLevel(const LevelRecord& record) {
    auto start = std::chrono::steady_clock::now();
    assert(m_screens.size() == 1);
    auto screen = std::make_unique<LevelScreen>(m_instance, record);
    m_screens.push_back(std::move(screen));
    if(m_instance.live)
        curScreen().start();
    m_state = GameState::Game;
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("startLevel duration = %f s", diff.count());
}

void StateManager::playIntro() {
    auto& jni = m_instance.jni();
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}