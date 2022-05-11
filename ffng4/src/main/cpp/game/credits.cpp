#include "credits.h"
#include "statemanager.h"

CreditsScreen::CreditsScreen(Instance& instance) : GameScreen(instance) {
    addImage("override/credits.png", "credits");
    addImage("override/credits-header.png", "header");
    m_music = m_instance.audio().loadMusic("music/kufrik.ogg");
    m_viewWidth = baseWidth;
    m_viewHeight = baseHeight;
    m_totalHeight = 0;
}

void CreditsScreen::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
}

void CreditsScreen::own_refresh() {
    m_totalHeight = getImage("credits").height();
}

void CreditsScreen::own_draw(const DrawTarget& target) {
    auto offset = (int)(std::min(timeAlive() / 50.0f, 1.0f) * (float) m_totalHeight);
    const auto& program = m_instance.graphics().shaders().copy;
    target.blit(getImage("credits"), program, 0, 0,
                0, offset - (int)m_viewHeight, m_viewWidth, m_viewHeight);
    target.blit(getImage("header"), program);
}

bool CreditsScreen::own_mouse(unsigned int x, unsigned int y) {
    m_instance.states().setState(GameState::WorldMap);
    return true;
}