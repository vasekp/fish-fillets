#include "testscreen.h"

TestScreen::TestScreen(Instance* instance, const char* a, const char* b, const char *c) : GameScreen(instance) {
    setBackground(a);
    addImage("walls", b);
    setMusic(c);
}

void TestScreen::own_load() {
    auto& program =m_instance->graphics->shaders()->wavyImage;
    glUseProgram(program);
    glUniform1f(program.uniform("uAmplitude"), 4.0);
    glUniform1f(program.uniform("uPeriod"), 12.0);
    glUniform1f(program.uniform("uSpeed"), 5.0);
    m_loadTime = std::chrono::steady_clock::now();
}

void TestScreen::own_draw() {
    const auto& canvas = m_instance->graphics->canvas();
    const auto& copyProgram = m_instance->graphics->shaders()->copy;
    const auto& wavyProgram = m_instance->graphics->shaders()->wavyImage;

    float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - m_loadTime).count();
    float phase = std::fmod(time, (float)(2*M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uShift"), phase);

    canvas->drawImage(getImage("background"), wavyProgram);
    canvas->drawImage(getImage("walls"), copyProgram);
}