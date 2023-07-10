#include "credits.h"
#include "screenmanager.h"

CreditsScreen::CreditsScreen(Instance& instance) :
        GameScreen(instance),
        m_input(instance, *this)
{
    auto* cImage = addImage("images/menu/credits.png", "credits");
    addImage("images/menu/credits-header.png", "header");
    m_music = m_instance.audio().loadMusic("music/kufrik.ogg");
    m_totalHeight = cImage->height();
}

void CreditsScreen::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
}

void CreditsScreen::own_draw(const DrawTarget& target) {
    float offset = std::min(timeAlive() / 50.0f, 1.0f) * (float) m_totalHeight;
    const auto& program = m_instance.graphics().shaders().copy;
    target.blit(getImage("credits"), m_instance.graphics().coords(Graphics::CoordSystems::base), program, 0, 0,
                0, offset - Graphics::baseDim.fy(), Graphics::baseDim.x(), Graphics::baseDim.y());
    target.blit(getImage("header"), m_instance.graphics().coords(Graphics::CoordSystems::base), program);
}

bool CreditsScreen::own_pointer(FCoords) {
    quit();
    return true;
}

bool CreditsScreen::own_key(Key) {
    quit();
    return true;
}

void CreditsScreen::quit() {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
}
