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
            m_instance.input().reset();
            break;
        case Mode::Credits:
            m_screen = std::make_unique<CreditsScreen>(m_instance);
            curScreen().start();
            if(m_instance.live)
                curScreen().refresh();
            if(m_instance.running)
                curScreen().resume();
            m_instance.input().reset();
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

void ScreenManager::announceLevel(const LevelRecord& record) {
    m_title.emplace(m_instance, record.description.at("cs"));
    m_title_hide.reset();
    if(m_instance.live)
        m_title->refresh();
}

void ScreenManager::startLevel(LevelRecord& record) {
    auto start = std::chrono::steady_clock::now();
    m_screen = std::make_unique<LevelScreen>(m_instance, record);
    curScreen().start();
    if(m_instance.live)
        curScreen().refresh();
    if(m_instance.running)
        curScreen().resume();
    auto end = std::chrono::steady_clock::now();
    m_title_hide = end + std::chrono::milliseconds((int)titleDuration);
    m_instance.input().reset();
    std::chrono::duration<double> diff = end - start;
    Log::debug("startLevel duration = ", diff.count(), " s");
}

void ScreenManager::playIntro() {
    auto& jni = m_instance.jni();
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}

void ScreenManager::drawFrame() {
    auto& graphics = m_instance.graphics();
    if(!graphics.ready()) {
      Log::error("drawFrame called without active graphics subsystem");
        return;
    }

    if(options()) {
        const auto& [blur1, blur2] = graphics.blurTargets();
        const auto& blurProgram = graphics.shaders().blur;

        blur1.bind();
        curScreen().draw(blur1);

        glUseProgram(blurProgram);

        blur2.bind();
        glUniform2f(blurProgram.uniform("uDelta"), 1.f, 0.f);
        blur2.blit(blur1.texture(), blurProgram);

        graphics.windowTarget().bind();
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform2f(blurProgram.uniform("uDelta"), 0.f, 1.f);
        graphics.windowTarget().blit(blur2.texture(), blurProgram);
    } else {
        const auto& offscreen = graphics.offscreenTarget();
        const auto& copyProgram = graphics.shaders().copy;

        offscreen.bind();
        curScreen().draw(offscreen);

        graphics.windowTarget().bind();
        glClear(GL_COLOR_BUFFER_BIT);
        auto shift = curScreen().shift();
        graphics.windowTarget().blit(offscreen.texture(), copyProgram, shift.fx(), shift.fy());
    }

    if(!options()) {
        graphics.fullscreenTarget().bind();
        curScreen().drawOverlays(graphics.fullscreenTarget());
        if (m_title) {
            float opacity = 1.0;
            if (m_title_hide) {
                auto timeLeft = std::chrono::duration<float>(
                        m_title_hide.value() - std::chrono::steady_clock::now()).count();
                opacity = std::clamp(3.f * timeLeft, 0.f, 1.f);
            }
            if (opacity == 0.f)
                m_title.reset();
            else {
                m_title->draw(graphics.fullscreenTarget(), opacity);
            }
        }
    }

    graphics.system().display().swap();
}

void ScreenManager::refresh() {
    if(m_title)
        m_title->refresh();
    curScreen().refresh();
}

void ScreenManager::pause() {
    curScreen().pause();
}

void ScreenManager::resume() {
    curScreen().resume();
}

bool ScreenManager::dispatchPointer(FCoords coords, bool longPress) {
    return curScreen().pointer(coords, longPress);
}

bool ScreenManager::dispatchKey(Key key) {
    return curScreen().keypress(key);
}
