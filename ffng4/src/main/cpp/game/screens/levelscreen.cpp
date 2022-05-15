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
    m_level.layout().update(dt);
    if(m_timer.ticked())
        m_level.tick();

    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& overlayProgram = m_instance.graphics().shaders().copyOverlay;
    const auto& wavyProgram = m_instance.graphics().shaders().wavyImage;

    float phase = std::fmod(timeAlive(), (float)(2 * M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uPhase"), phase);

    target.blit(getImage("background"), wavyProgram);

    for(const auto& uModel : m_level.layout().models()) {
        const auto& model = *uModel;
        if(model.isVirtual())
            continue;
        const auto& images = model.anim().get();
        if(m_effects.contains(&model)) {
            if(images.size() > 1)
                LOGE("layered image x record");
            auto& record = m_effects[&model];
            auto& program = m_instance.graphics().shaders().*record.effect;
            glUseProgram(program);
            glUniform1f(program.uniform("uTime"), timeAlive() - record.startTime);
            target.blit(images[0], program, model.fx() * size_unit, model.fy() * size_unit);
        } else {
            for (auto i = 0u; i < images.size(); i++)
                target.blit(images[i], i == 0 ? copyProgram : overlayProgram,
                            model.fx() * size_unit, model.fy() * size_unit);
        }
    }
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

void LevelScreen::setEffect(Model *model, const std::string &name) {
    ogl::Program Shaders::*effect;
    if(name == "disintegrate")
        effect = &Shaders::disintegrate;
    else if(name == "mirror")
        effect = &Shaders::mirror;
    else {
        LOGE("Unhandled setEffect %s", name.c_str());
        return;
    }
    m_effects[model] = {effect, timeAlive()};
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
    if(m_level.blocked())
        return false;
    switch(key) {
        case Key::up:
        case Key::down:
        case Key::left:
        case Key::right:
        case Key::space:
            m_level.layout().keyInput(key);
            return true;
        case Key::exit:
            m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
            return true;
        default:
            return false;
    }
}

void LevelScreen::own_drawOverlays(const DrawTarget &target, float dTime, float absTime) {
    m_subs.draw(target, dTime, absTime);
}
