#include "levelscreen.h"
#include "game/logic/level.h"
#include "screenmanager.h"

LevelScreen::LevelScreen(Instance& instance, LevelRecord& record) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_input(instance),
        m_waves(),
        m_subs(instance),
        m_fullLoad(false),
        m_quit(false)
{ }

void LevelScreen::restore() {
    m_display.reset();
    m_shift = {};
}

void LevelScreen::exit() {
    m_quit = true;
}

void LevelScreen::leave() {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
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

    {
        const auto& models = m_level.layout().models();
        auto it = std::find_if(models.begin(), models.end(), [&](const auto& model) { return model->effect().name == Model::Effect::mirror; });
        if(it == models.end())
            m_mirrorTarget = {};
        else
            m_mirrorTarget = makeMirrorTarget(**it);
    }

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

    if(m_quit) {
        leave();
        return;
    }

    const auto& copyProgram = m_instance.graphics().shaders().copy;
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

    for(const auto& rope : m_level.layout().getRopes()) {
        FCoords c1 = rope.m1->fxy() * size_unit + rope.d1;
        FCoords c2 = rope.m2->fxy() * size_unit + rope.d2;
        target.fill(ropeProgram, rope.m1->fx() * size_unit + (float)rope.d1.x, c1.fy(), std::max(c1.fx(), c2.fx()) + 1.f, c2.fy());
    }

    const Model* mirror = nullptr;
    for(const auto& uModel : m_level.layout().models()) {
        const auto& model = *uModel;
        if(model.isVirtual())
            continue;
        const auto images = model.anim().get(model.orientation());
        auto [effect, effectTime] = model.effect();
        switch(effect) {
            case Model::Effect::none:
                for(const auto* image : images)
                    target.blit(image, copyProgram, model.fx() * size_unit, model.fy() * size_unit);
                break;
            case Model::Effect::invisible:
                break;
            case Model::Effect::mirror:
                mirror = &model;
                break;
            case Model::Effect::reverse:
                for(const auto* image : images)
                    target.blit(image, m_instance.graphics().shaders().reverse, model.fx() * size_unit, model.fy() * size_unit);
                break;
            case Model::Effect::disintegrate: {
                auto& program = m_instance.graphics().shaders().disintegrate;
                glUseProgram(program);
                glUniform1f(program.uniform("uTime"), timeAlive() - effectTime);
                for(const auto* image : images)
                    target.blit(image, program, model.fx() * size_unit, model.fy() * size_unit);
                break;
            }
        }
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

void LevelScreen::killAllSounds() {
    m_instance.audio().clear();
    if(m_music)
        m_instance.audio().addSource(m_music);
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
                leave();
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