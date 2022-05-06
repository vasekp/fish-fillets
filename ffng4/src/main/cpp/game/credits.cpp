#include "credits.h"
#include "statemanager.h"

CreditsScreen::CreditsScreen(Instance& instance) : GameScreen(instance) {
    setBackground("override/credits-header.png"); // need background for sizing
    addImage("override/credits.png", "credits");
    setMusic("music/kufrik.ogg");
    m_viewWidth = 0;
    m_viewHeight = 0;
    m_totalHeight = 0;
}

void CreditsScreen::own_load() {
    m_viewWidth = getImage("background").width();
    m_viewHeight = getImage("background").height();
    m_totalHeight = getImage("credits").height();
}

void CreditsScreen::own_draw() {
    auto offset = (int)(std::min(timeSinceLoad() / 50.0f, 1.0f) * (float) m_totalHeight);
    const auto& canvas = m_instance.graphics().canvas();
    const auto& program = m_instance.graphics().shaders().copy;
    canvas.blit(getImage("credits"), program, 0, offset - (int)m_viewHeight, 0, 0, m_viewWidth, m_viewHeight);
    canvas.drawImage(getImage("background"), program);
}

bool CreditsScreen::own_mouse(unsigned int x, unsigned int y) {
    m_instance.states().setState(GameState::WorldMap);
    return true;
}