#include "screenmanager.h"
#include "game/screens/worldmap.h"
#include "game/screens/credits.h"
#include "game/screens/levelscreen.h"

GameScreen& ScreenManager::curScreen() {
    return *m_screen;
}

void ScreenManager::startMode(Mode mode) {
    auto start = std::chrono::steady_clock::now();
    switch(mode) {
        case Mode::WorldMap:
            m_screen = std::make_unique<WorldMap>(m_instance);
            curScreen().start();
            if(m_instance.live)
                curScreen().refresh();
            if(m_instance.running)
                curScreen().resume();
            m_instance.inputSource().reset();
            break;
        case Mode::Credits:
            m_screen = std::make_unique<CreditsScreen>(m_instance);
            curScreen().start();
            if(m_instance.live)
                curScreen().refresh();
            if(m_instance.running)
                curScreen().resume();
            m_instance.inputSource().reset();
            break;
        case Mode::Intro:
            playIntro();
            // Don't set m_screen.
            return;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    Log::debug("startMode duration = ", diff.count(), " s");
}

void ScreenManager::announceLevel(const std::string& title) {
    if(!title.empty())
        m_title.show(title);
    else
        m_title.hide();
}

Level& ScreenManager::startLevel(LevelRecord& record) {
    auto start = std::chrono::steady_clock::now();
    auto screen = std::make_unique<LevelScreen>(m_instance, record);
    auto& level = screen->level();
    m_screen = std::move(screen);
    curScreen().start();
    if(m_instance.live)
        curScreen().refresh();
    if(m_instance.running)
        curScreen().resume();
    auto end = std::chrono::steady_clock::now();
    m_title.fadeout();
    m_instance.inputSource().reset();
    std::chrono::duration<double> diff = end - start;
    Log::debug("startLevel duration = ", diff.count(), " s");
    return level;
}

void ScreenManager::drawFrame() {
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

    if(options()) {
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
    } else {
        graphics.fullscreenTarget().bind();
        graphics.fullscreenTarget().blit(offscreen.texture(), coords, copyProgram);
    }

    graphics.system().display().swap();
}

void ScreenManager::refresh() {
    curScreen().refresh();
    m_title.refresh();
}

void ScreenManager::pause() {
    curScreen().pause();
}

void ScreenManager::resume() {
    curScreen().resume();
}
