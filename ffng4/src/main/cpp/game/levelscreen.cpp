#include "levelscreen.h"
#include "level.h"
#include "statemanager.h"

LevelScreen::LevelScreen(Instance& instance, const LevelRecord& record) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_sounds(),
        m_music(),
        m_waves()
{ }

void LevelScreen::create(int width, int height, const std::string &background) {
    setSize(width * size_unit, height * size_unit);
    addImage(background, "background");
}

void LevelScreen::own_start() {
    m_level.init();

    m_instance.audio().clear();
    if(m_music)
        m_instance.audio().addSource(m_music);
}

void LevelScreen::own_refresh() {
    auto& program = m_instance.graphics().shaders().wavyImage;
    glUseProgram(program);
    glUniform1f(program.uniform("uAmplitude"), m_waves[0]);
    glUniform1f(program.uniform("uPeriod"), m_waves[1]);
    glUniform1f(program.uniform("uSpeed"), m_waves[2]);
}

void LevelScreen::own_draw() {
    if(m_timer.ticked())
        m_level.tick();

    const auto& canvas = m_instance.graphics().canvas();
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& overlayProgram = m_instance.graphics().shaders().copyOverlay;
    const auto& wavyProgram = m_instance.graphics().shaders().wavyImage;

    float phase = std::fmod(timeAlive(), (float)(2 * M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uPhase"), phase);

    canvas.drawImage(getImage("background"), wavyProgram);

    for(const auto& model : m_level.models()) {
        const auto& images = model.anim().get();
        for(auto i = 0u; i < images.size(); i++)
            canvas.drawImage(images[i], i == 0 ? copyProgram : overlayProgram, model.x() * size_unit, model.y() * size_unit);
    }
}

void LevelScreen::addSound(const std::string &name, const std::string &filename) {
    m_sounds.insert({name, m_instance.audio().loadSound(filename)});
}

void LevelScreen::setWaves(float amplitude, float period, float speed) {
    m_waves = {amplitude, period, speed};
}

void LevelScreen::playMusic(const std::string &filename) {
    auto x = m_instance.audio().loadMusic(filename);
    m_music = x;
}

void LevelScreen::own_pause() {
    m_timer.stop();
}

void LevelScreen::own_resume() {
    m_timer.start();
}

bool LevelScreen::own_mouse(unsigned int x, unsigned int y) {
    m_instance.states().setState(GameState::WorldMap);
    return true;
}