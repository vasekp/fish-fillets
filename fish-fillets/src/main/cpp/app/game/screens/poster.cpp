#include "poster.h"
#include "screenmanager.h"
#include "subsystem/persist.h"

PosterScreen::PosterScreen(Instance& instance, const std::string& script) :
        GameScreen(instance),
        m_input(instance, *this),
        m_script(instance, *this)
{
    m_music = m_instance.audio().loadMusic("music/rybky11.ogg", false);
    m_script.registerFn("poster_display", lua::wrap<&PosterScreen::poster_display>);
    m_script.registerFn("poster_translation", lua::wrap<&PosterScreen::poster_translation>);
    m_script.loadFile(script);
}

void PosterScreen::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
}

void PosterScreen::own_draw(DrawTarget& target) {
    const auto program = m_instance.graphics().shaders().copy();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    target.draw(getImage("poster"), program, coords);
    if(m_hint)
        m_hint->draw(target);
}

bool PosterScreen::own_pointer(FCoords) {
    if(liveTime().time_since_epoch() > minTime) {
        quit();
        return true;
    } else
        return false;
}

bool PosterScreen::own_key(Key) {
    if(liveTime().time_since_epoch() > minTime) {
        quit();
        return true;
    } else
        return false;
}

void PosterScreen::own_resize() {
    if(m_hint)
        m_hint->resize();
}

void PosterScreen::quit() {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
}

void PosterScreen::poster_display(const std::string& filename) {
    addImage(filename, "poster");
}

void PosterScreen::poster_translation(const std::string& text) {
    auto lang = m_instance.persist().get("subtitles", "cs"s);
    if(lang != "cs")
        m_hint.emplace(m_instance, text, true);
}
