#include "testscreen.h"

TestScreen::TestScreen(Instance* instance, const char* a, const char* b) : GameScreen(instance) {
    setBackground(a);
    setMusic(b);
}

void TestScreen::own_draw() {
    const auto& canvas = m_instance->graphics->canvas();
    const auto& copyProgram = m_instance->graphics->shaders()->copy;

    canvas->drawImage(getImage("background"), copyProgram);
}