#include "screenmanager.h"
#include "game/screens/worldmap.h"
#include "game/screens/credits.h"
#include "game/screens/intro.h"
#include "game/screens/levelscreen.h"

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
    auto& level = open<LevelScreen>(record, replay).level();
    m_title.fadeout();
}

void ScreenManager::useNext() {
    m_screen = std::move(m_next);
    curScreen().start();
    if(m_instance.running)
        curScreen().resume();
    m_instance.inputSource().reset();
}

void ScreenManager::drawFrame() {
    if(m_next)
        useNext();

    auto& graphics = m_instance.graphics();
    if(!graphics.ready()) {
      Log::error("drawFrame called without active graphics subsystem");
        return;
    }
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::null);

    const auto& offscreen = graphics.offscreenTarget();
    const auto& copyProgram = graphics.shaders().copy;
    offscreen.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    curScreen().draw(offscreen);

    curScreen().drawOverlays(offscreen);
    m_title.draw(offscreen);

    if(m_options.visible()) {
        const auto& [blur1, blur2] = graphics.blurTargets();
        const auto& blurProgram = graphics.shaders().blur;

        blur1.bind();
        blur1.blit(offscreen.texture(), coords, copyProgram);

        blur2.bind();
        glUseProgram(blurProgram);
        glUniform2f(blurProgram.uniform("uDelta"), 1.f, 0.f);
        blur2.blit(blur1.texture(), coords, blurProgram);

        graphics.fullscreenTarget().bind();
        glUniform2f(blurProgram.uniform("uDelta"), 0.f, 1.f);
        graphics.fullscreenTarget().blit(blur2.texture(), coords, blurProgram);

        m_options.draw(graphics.fullscreenTarget());
    } else {
        graphics.fullscreenTarget().bind();
        graphics.fullscreenTarget().blit(offscreen.texture(), coords, copyProgram);
    }

    graphics.system().display().swap();
}

IInputSink& ScreenManager::input() {
    return m_options.visible() ? m_options.input() : curScreen().input();
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
