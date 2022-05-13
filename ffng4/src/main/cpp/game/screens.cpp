#include "screens.h"
#include "game/worldmap.h"
#include "game/credits.h"
#include "game/levelscreen.h"

GameScreen& Screens::curScreen() {
    return *m_screen;
}

void Screens::startMode(Mode mode) {
    auto start = std::chrono::steady_clock::now();
    switch(mode) {
        case Mode::WorldMap:
            m_screen = std::make_unique<WorldMap>(m_instance);
            curScreen().start();
            if(m_instance.live) {
                curScreen().refresh();
                curScreen().resume();
            }
            break;
        case Mode::Credits:
            m_screen = std::make_unique<CreditsScreen>(m_instance);
            curScreen().start();
            if(m_instance.live) {
                curScreen().refresh();
                curScreen().resume();
            }
            break;
        case Mode::Intro:
            playIntro();
            // Don't set m_screen.
            return;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("startMode duration = %f s", diff.count());
}

void Screens::startLevel(const LevelRecord& record) {
    auto start = std::chrono::steady_clock::now();
    m_screen = std::make_unique<LevelScreen>(m_instance, record);
    curScreen().start();
    if(m_instance.live) {
        curScreen().refresh();
        curScreen().resume();
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    LOGD("startLevel duration = %f s", diff.count());
}

void Screens::playIntro() {
    auto& jni = m_instance.jni();
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}

void Screens::drawFrame() {
    auto& graphics = m_instance.graphics();
    if(!graphics.ready()) {
        LOGE("drawFrame called without active graphics subsystem");
        return;
    }

    if(options()) {
        const auto& blur1 = graphics.offscreenTarget(GraphicsSystem::Targets::Blur1);
        const auto& blur2 = graphics.offscreenTarget(GraphicsSystem::Targets::Blur2);
        const auto& blurProgram = graphics.shaders().blur;

        blur1.bind();
        curScreen().draw(blur1);

        glUseProgram(blurProgram);

        blur2.bind();
        glUniform2f(blurProgram.uniform("uDelta"), 1.f, 0.f);
        blur2.blitFlip(blur1.texture(), blurProgram, false, true);

        graphics.windowTarget().bind();
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform2f(blurProgram.uniform("uDelta"), 0.f, 1.f);
        graphics.windowTarget().blitFlip(blur2.texture(), blurProgram, false, true);
    } else {
        graphics.windowTarget().bind();
        glClear(GL_COLOR_BUFFER_BIT);
        curScreen().draw(graphics.windowTarget());
    }

    graphics.system().display().swap();
}

void Screens::refresh() {
    curScreen().refresh();
}

void Screens::pause() {
    curScreen().pause();
}

void Screens::resume() {
    curScreen().resume();
}

bool Screens::mouse(Coords coords) {
    return curScreen().mouse(coords);
}
