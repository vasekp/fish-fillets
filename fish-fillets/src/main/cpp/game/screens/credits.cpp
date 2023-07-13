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

void CreditsScreen::own_draw(DrawTarget& target) {
    float offset = std::min(timeAlive() / 50.0f, 1.0f) * (float) m_totalHeight;
    const auto& program = m_instance.graphics().shaders().copy;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    target.draw(getImage("credits"), program, coords, {
        .src = FCoords{0.f, offset - Graphics::baseDim.fy()},
        .area = Graphics::baseDim
    });
    target.draw(getImage("header"), program, coords);
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
