#include "intro.h"
#include "screenmanager.h"

IntroScreen::IntroScreen(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this)
{
}

void IntroScreen::own_start() {
    m_instance.audio().clear();
    m_instance.screens().announceLevel("");
}

void IntroScreen::own_draw(const DrawTarget& target, float dt) {
}

bool IntroScreen::own_pointer(FCoords coords, bool longPress) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}

bool IntroScreen::own_key(Key key) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}
