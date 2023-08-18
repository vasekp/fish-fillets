#include "screenmanager.h"
#include "game/screens/worldmap.h"
#include "game/screens/credits.h"
#include "game/screens/intro.h"
#include "game/screens/levelscreen.h"
#include "game/screens/poster.h"

void ScreenManager::startMode(Mode mode) {
    switch(mode) {
        case Mode::WorldMap:
            open<WorldMap>();
            break;
        case Mode::Credits:
            open<CreditsScreen>();
            break;
        case Mode::Intro:
            open<IntroScreen>();
            return;
    }
}

template<class ScreenType, typename... Ts>
ScreenType& ScreenManager::open(Ts&&... ts) {
    auto start = std::chrono::steady_clock::now();
    auto ptr = std::make_unique<ScreenType>(m_instance, std::forward<Ts>(ts)...);
    auto& screen = *ptr;
    m_next = std::move(ptr);
    if(!m_screen)
        useNext();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    Log::debug<Log::lifecycle>("ScreenManager::open duration = ", diff.count(), " s");
    return screen;
}

void ScreenManager::announceLevel(const std::string& title) {
    if(!title.empty())
        m_title.show(title);
    else
        m_title.hide();
}

void ScreenManager::startLevel(LevelRecord& record, bool replay) {
    open<LevelScreen>(record, replay).level();
    m_title.fadeout();
}

void ScreenManager::poster(const LevelRecord& record) {
    open<PosterScreen>(record.script_ending);
}

void ScreenManager::useNext() {
    m_screen = std::move(m_next);
    curScreen().start();
    if(m_instance.running())
        curScreen().resume();
    m_instance.inputSource().reset();
}

void ScreenManager::updateAll() {
    if(m_next)
        useNext();
    curScreen().update();
    m_title.update();
}

void ScreenManager::drawFrame() {
    auto& graphics = m_instance.graphics();
    const auto& coords = graphics.coords(Graphics::CoordSystems::null);
    auto& offscreen = graphics.offscreenTarget();
    const auto copyProgram = graphics.shaders().copy();
    graphics.newFrame();
    curScreen().draw(offscreen);
    m_title.draw(offscreen);
    if(m_options.visible()) {
        auto& [blur1, blur2] = graphics.blurTargets();
        blur1.draw(offscreen.texture(), copyProgram, coords);
        blur2.draw(blur1.texture(), graphics.shaders().blur({ .dir = FCoords{1.f, 0.f} }), coords);
        offscreen.draw(blur2.texture(), graphics.shaders().blur({ .dir = FCoords{0.f, 1.f} }), coords);
        m_options.draw(offscreen);
    }
    graphics.present(offscreen);
}

IInputSink& ScreenManager::input() {
    return m_options.visible() ? m_options : curScreen().input();
}

IInputSource& ScreenManager::inputSourceMasked() {
    return m_options.visible() ? nullInputSource : m_instance.inputSource();
}

void ScreenManager::resize() {
    curScreen().resize();
    m_title.resize();
}

void ScreenManager::pause() {
    curScreen().pause();
}

void ScreenManager::resume() {
    curScreen().resume();
}
