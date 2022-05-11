#include "screens.h"
#include "game/worldmap.h"
#include "game/credits.h"
#include "game/levelscreen.h"

void Screens::startMode(Mode mode) {
    auto start = std::chrono::steady_clock::now();
    switch(mode) {
        case Mode::WorldMap:
            m_screen = std::make_unique<WorldMap>(m_instance);
            curScreen().start();
            if(m_instance.live) {
                curScreen().refresh();
                curScreen().resume();
            }
            break;
        case Mode::Credits:
            m_screen = std::make_unique<CreditsScreen>(m_instance);
            curScreen().start();
            if(m_instance.live) {
                curScreen().refresh();
                curScreen().resume();
            }
            break;
        case Mode::Intro:
            playIntro();
            // Don't set m_screen.
            return;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("startMode duration = %f s", diff.count());
}

void Screens::startLevel(const LevelRecord& record) {
    auto start = std::chrono::steady_clock::now();
    m_screen = std::make_unique<LevelScreen>(m_instance, record);
    curScreen().start();
    if(m_instance.live) {
        curScreen().refresh();
        curScreen().resume();
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("startLevel duration = %f s", diff.count());
}

void Screens::playIntro() {
    auto& jni = m_instance.jni();
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}