#include "levelscreen.h"
#include "game/logic/level.h"
#include "screenmanager.h"

LevelScreen::LevelScreen(Instance& instance, const LevelRecord& record) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_input(instance),
        m_waves(),
        m_subs(instance),
        m_fullLoad(false)
{
    m_input.setSavePossible(m_level.savePossible());
    m_input.setLoadPossible(m_level.loadPossible());
}

void LevelScreen::restore() {
    m_display.reset();
    m_effects.clear();
    m_shift = {};
}

void LevelScreen::own_start() {
    m_level.init();
}

void LevelScreen::own_setsize() {
    FCoords displaySize{m_instance.graphics().display().width(), m_instance.graphics().display().height()};
    FCoords levelSize{m_level.layout().width(), m_level.layout().height()};
    m_input.setButtonGravity(levelSize.fx() / levelSize.fy() > displaySize.fx() / displaySize.fy()
        ? LevelInput::ButtonGravity::top
        : LevelInput::ButtonGravity::left);
    m_input.refresh();
    m_instance.graphics().setWindowSize(m_level.layout().width() * size_unit, m_level.layout().height() * size_unit, m_input.getReserve());
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

    if(m_display)
        m_display->reload(m_instance);

    const auto& models = m_level.layout().models();
    auto it = std::find_if(models.begin(),  models.end(), [&](const auto& model) {
        return m_effects.contains(model) && m_effects.at(model).effect == &Shaders::mirror;
    });
    if(it == models.end())
        m_mirrorTarget = {};
    else
        m_mirrorTarget = makeMirrorTarget(**it);

    glUseProgram(m_instance.graphics().shaders().rope);
    glUniform4fv(m_instance.graphics().shaders().rope.uniform("uColor"), 1, Color(0x30404E).gl().get());
}

std::unique_ptr<TextureTarget> LevelScreen::makeMirrorTarget(const Model &model) {
    FCoords modelSizePixel = size_unit * FCoords{model.shape().width(), model.shape().height()};
    FCoords modelSizeNative = m_instance.graphics().windowTarget().pixelScale() * modelSizePixel;
    auto ret = std::make_unique<TextureTarget>(m_instance.graphics().system().ref());
    ret->resize(modelSizeNative.x(), modelSizeNative.y(), modelSizePixel.fx(), modelSizePixel.fy());
    return ret;
}

void LevelScreen::own_draw(const DrawTarget& target, float dt) {
    m_level.layout().animate(dt);
    m_level.rules().update();
    if(m_timer.ticked())
        m_level.tick();

    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& overlayProgram = m_instance.graphics().shaders().copyOverlay;
    const auto& wavyProgram = m_instance.graphics().shaders().wavyImage;
    const auto& ropeProgram = m_instance.graphics().shaders().rope;

    if(m_display) {
        target.blit(&m_display.value(), copyProgram);
        return;
    }

    float phase = std::fmod(timeAlive(), (float)(2 * M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uPhase"), phase);

    target.blit(getImage("background"), wavyProgram);

    const Model* mirror = nullptr;
    for(const auto& uModel : m_level.layout().models()) {
        const auto& model = *uModel;
        if(model.isVirtual())
            continue;
        const auto images = model.anim().get(model.orientation());
        if(m_effects.contains(&model)) {
            if(m_effects.at(&model).effect == &Shaders::mirror) {
                mirror = &model;
                continue;
            }
            if(images.size() > 1)
                LOGE("layered image x effect");
            auto& record = m_effects[&model];
            auto& program = m_instance.graphics().shaders().*record.effect;
            glUseProgram(program);
            glUniform1f(program.uniform("uTime"), timeAlive() - record.startTime);
            target.blit(images[0], program, model.fx() * size_unit, model.fy() * size_unit);
        } else {
            for (auto i = 0u; i < images.size(); i++)
                target.blit(images[i], i == 0 ? copyProgram : overlayProgram, model.fx() * size_unit, model.fy() * size_unit);
        }
    }

    for(const auto& rope : m_level.layout().getRopes()) {
        FCoords c1 = rope.m1->fxy() * size_unit + rope.d1;
        FCoords c2 = rope.m2->fxy() * size_unit + rope.d2;
        target.fill(ropeProgram, rope.m1->fx() * size_unit + (float)rope.d1.x, c1.fy(), std::max(c1.fx(), c2.fx()) + 1.f, c2.fy());
    }

    if(mirror) {
        auto* image =  mirror->anim().get(mirror->orientation())[0];
        m_instance.graphics().setMask(image);
        m_mirrorTarget->bind();
        m_mirrorTarget->blit(m_instance.graphics().offscreenTarget().texture(), m_instance.graphics().shaders().mirror,
                                 0, 0, size_unit * mirror->fx(), size_unit * mirror->fy());
        target.bind();
        target.blit(m_mirrorTarget->texture(), copyProgram, mirror->fx() * size_unit, mirror->fy() * size_unit);
    }
}

AudioSourceRef LevelScreen::addSound(const std::string &name, const std::string &filename, bool single) {
    if(single && m_sounds.contains(name))
        return m_sounds.find(name)->second;
    auto it = m_sounds.insert({name, m_instance.audio().loadSound(filename)});
    return it->second;
}

void LevelScreen::addSubtitle(const std::string &text, const std::string& colors) {
    m_subs.add(text, colors);
}

void LevelScreen::setWaves(float amplitude, float period, float speed) {
    m_waves = {amplitude, period, speed};
}

void LevelScreen::playMusic(const std::string &filename) {
    if(m_fullLoad)
        stopMusic();
    m_music = m_instance.audio().loadMusic(filename);
    if(m_fullLoad)
        m_instance.audio().addSource(m_music);
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

void LevelScreen::setShift(FCoords shift) {
    m_shift = shift;
}

void LevelScreen::own_pause() {
    m_timer.stop();
}

void LevelScreen::own_resume() {
    m_timer.start();
}

bool LevelScreen::own_mouse(unsigned int x, unsigned int y) {
//    TODO: switch fish
    return false;
}

bool LevelScreen::own_key(Key key) {
    switch(key) {
        case Key::up:
        case Key::down:
        case Key::left:
        case Key::right:
        case Key::space:
            if(m_level.accepting()) {
                m_level.rules().keyInput(key);
                return true;
            } else
                return false;
        case Key::exit:
            if(!m_level.quitDemo())
                m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
            return true;
        case Key::save:
            m_level.save();
            return true;
        case Key::load:
            m_level.load();
            return true;
        case Key::restart:
            m_level.restart();
            return true;
        default:
            return false;
    }
}

void LevelScreen::own_drawOverlays(const DrawTarget &target, float dTime, float absTime) {
    m_subs.draw(target, dTime, absTime);
    m_input.draw(target);
}

FCoords LevelScreen::shift() {
    return m_shift;
}

void LevelScreen::display(const std::string& filename) {
    if(!filename.empty())
        m_display.emplace(m_instance.files().system(filename), m_instance);
    else
        m_display.reset();
}

