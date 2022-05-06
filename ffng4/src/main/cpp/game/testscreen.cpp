#include "testscreen.h"
#include "statemanager.h"

TestScreen::TestScreen(Instance& instance, const char* a, const char* b, const char *c) : GameScreen(instance) {
    setBackground(a);
    addImage(b, "walls");
    setMusic(c);
    for(int i = 0; i < 6; i++) {
        std::stringstream oss;
        oss << "sound/share/sp-bubles_0" << i << ".ogg";
        m_sound[i] = instance.audio().loadSound(oss.str());
    }
}

void TestScreen::own_load() {
    auto& program =m_instance.graphics().shaders().wavyImage;
    glUseProgram(program);
    glUniform1f(program.uniform("uAmplitude"), 4.0);
    glUniform1f(program.uniform("uPeriod"), 12.0);
    glUniform1f(program.uniform("uSpeed"), 5.0);
}

void TestScreen::own_draw() {
    const auto& canvas = m_instance.graphics().canvas();
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& wavyProgram = m_instance.graphics().shaders().wavyImage;

    float phase = std::fmod(timeSinceLoad(), (float)(2*M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uPhase"), phase);

    canvas.drawImage(getImage("background"), wavyProgram);
    canvas.drawImage(getImage("walls"), copyProgram);
}

bool TestScreen::own_mouse(unsigned int x, unsigned int y) {
    m_instance.states().setState(GameState::WorldMap);
    static int j = 0;
    auto& sound = m_sound[j++];
    if(j == 6)
        j = 0;
    m_instance.audio().addSource(sound);
    return true;
}