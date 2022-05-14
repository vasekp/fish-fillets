#include "levelscreen.h"
#include "game/logic/level.h"
#include "screenmanager.h"

LevelScreen::LevelScreen(Instance& instance, const LevelRecord& record) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_waves(),
        m_subs(instance),
        m_fullLoad(false)
{ }

void LevelScreen::create(int width, int height, const std::string &background) {
    setSize(width * size_unit, height * size_unit);
    addImage(background, "background");
}

void LevelScreen::own_start() {
    m_level.init();
}

void LevelScreen::own_refresh() {
    auto& program = m_instance.graphics().shaders().wavyImage;
    glUseProgram(program);
    glUniform1f(program.uniform("uAmplitude"), m_waves[0]);
    glUniform1f(program.uniform("uPeriod"), m_waves[1]);
    glUniform1f(program.uniform("uSpeed"), m_waves[2]);
    m_subs.refresh();
    if(!m_fullLoad) {
        m_fullLoad = true;
        m_instance.audio().clear();
        if(m_music)
            m_instance.audio().addSource(m_music);
    }
}

void LevelScreen::own_draw(const DrawTarget& target, float dt) {
    if(m_timer.ticked())
        m_level.tick();

    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& overlayProgram = m_instance.graphics().shaders().copyOverlay;
    const auto& wavyProgram = m_instance.graphics().shaders().wavyImage;

    float phase = std::fmod(timeAlive(), (float)(2 * M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uPhase"), phase);

    target.blit(getImage("background"), wavyProgram);

    for(const auto& model : m_level.models()) {
        if(model->isVirtual())
            continue;
        const auto& images = model->anim().get();
        for(auto i = 0u; i < images.size(); i++)
            target.blit(images[i], i == 0 ? copyProgram : overlayProgram, model->x() * size_unit, model->y() * size_unit);
    }

    m_subs.draw(target, dt, timeAlive());
}

AudioSource& LevelScreen::addSound(const std::string &name, const std::string &filename, bool single) {
    if(single && m_sounds.contains(name))
        return m_sounds.find(name)->second;
    auto it = m_sounds.insert({name, m_instance.audio().loadSound(filename)});
    return it->second;
}

void LevelScreen::addSubtitle(const std::string &text, const std::string& colors) {
    m_subs.add(text, colors, timeAlive());
}

void LevelScreen::setWaves(float amplitude, float period, float speed) {
    m_waves = {amplitude, period, speed};
}

void LevelScreen::playMusic(const std::string &filename) {
    m_music = m_instance.audio().loadMusic(filename);
    if(m_fullLoad) {
        stopMusic();
        m_instance.audio().addSource(m_music);
    }
}

void LevelScreen::stopMusic() {
    m_instance.audio().removeSource(m_music);
    m_music.reset();
}

void LevelScreen::own_pause() {
    m_timer.stop();
}

void LevelScreen::own_resume() {
    m_timer.start();
}

bool LevelScreen::own_mouse(unsigned int x, unsigned int y) {
//    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}

bool LevelScreen::own_key(Key key) {
    switch(key) {
        case Key::up:
            m_level.moveFish(Displacement::up);
            return true;
        case Key::down:
            m_level.moveFish(Displacement::down);
            return true;
        case Key::left:
            m_level.moveFish(Displacement::left);
            return true;
        case Key::right:
            m_level.moveFish(Displacement::right);
            return true;
        case Key::space:
            m_level.switchFish();
            return true;
        case Key::exit:
            m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
            return true;
        default:
            return false;
    }
}
