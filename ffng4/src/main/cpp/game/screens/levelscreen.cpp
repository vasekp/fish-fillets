#include "levelscreen.h"
#include "game/logic/level.h"
#include "screenmanager.h"
#include "subsystem/rng.h"

LevelScreen::LevelScreen(Instance& instance, LevelRecord& record) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_input(instance, *this),
        m_waves(),
        m_winSize(),
        m_subs(instance),
        m_quit(false),
        m_flashAlpha(0)
{ }

void LevelScreen::restore() {
    m_display.reset();
    m_instance.graphics().setWindowShift({});
}

void LevelScreen::exit() {
    m_quit = true;
}

void LevelScreen::leave() {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
}

void LevelScreen::own_start() {
    m_instance.audio().clear();
    m_level.init();
}

void LevelScreen::own_resize() {
    m_winSize = FCoords{m_level.layout().width(), m_level.layout().height()} * size_unit;
    m_instance.graphics().setWindowSize(m_winSize.x(), m_winSize.y());
    m_input.resize();
    m_subs.resize();

    const auto& models = m_level.layout().models();
    auto it = std::find_if(models.begin(), models.end(), [&](const auto& model) { return model->effect().name == Model::Effect::mirror; });
    if(it == models.end())
        m_mirrorTarget = {};
    else
        m_mirrorTarget = makeMirrorTarget(**it);
}

std::unique_ptr<TextureTarget> LevelScreen::makeMirrorTarget(const Model &model) { // TODO
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    FCoords modelSizePixel = coords.in2out_dim(model.size() * size_unit);
    auto ret = std::make_unique<TextureTarget>(m_instance.graphics().system().ref());
    ret->resize(modelSizePixel.x(), modelSizePixel.y());
    return ret;
}

void LevelScreen::own_draw(const DrawTarget& target, float dt) {
    m_level.update(dt);

    if(m_quit) {
        leave();
        return;
    }

    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& wavyProgram = m_instance.graphics().shaders().wavyImage;
    const auto& flatProgram = m_instance.graphics().shaders().flat;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);

    if(m_display) {
        target.blit(&m_display.value(), coords, copyProgram);
        return;
    }

    {
        FCoords from = coords.in2out({0, 0});
        FCoords to = coords.in2out(m_winSize);
        FCoords size = to - from;
        glScissor(from.x(), from.y(), size.x(), size.y());
        glEnable(GL_SCISSOR_TEST);
    }

    float phase = std::fmod(timeAlive(), (float)(2 * M_PI));
    glUseProgram(wavyProgram);
    glUniform1f(wavyProgram.uniform("uAmplitude"), m_waves[0]);
    glUniform1f(wavyProgram.uniform("uPeriod"), m_waves[1]);
    glUniform1f(wavyProgram.uniform("uSpeed"), m_waves[2]);
    glUniform1f(wavyProgram.uniform("uPhase"), phase);

    target.blit(getImage("background"), coords, wavyProgram);

    for(const auto& rope : m_level.layout().getRopes()) {
        constexpr Color ropeColor{0x30404E};
        FCoords c1 = rope.m1->fxy() * size_unit + rope.d1;
        FCoords c2 = rope.m2->fxy() * size_unit + rope.d2;
        glUseProgram(flatProgram);
        glUniform4fv(flatProgram.uniform("uColor"), 1, ropeColor.gl().data());
        target.fill(coords, flatProgram, rope.m1->fx() * size_unit + (float)rope.d1.x, c1.fy(), std::max(c1.fx(), c2.fx()) + 1.f, c2.fy());
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
                    target.blit(image, coords, copyProgram, model.fx() * size_unit, model.fy() * size_unit);
                break;
            case Model::Effect::invisible:
                break;
            case Model::Effect::mirror:
                mirror = &model;
                break;
            case Model::Effect::reverse:
                for(const auto* image : images)
                    target.blit(image, coords, m_instance.graphics().shaders().reverse, model.fx() * size_unit, model.fy() * size_unit);
                break;
            case Model::Effect::disintegrate: {
                auto& program = m_instance.graphics().shaders().disintegrate;
                glUseProgram(program);
                glUniform1f(program.uniform("uTime"), timeAlive() - effectTime);
                for(const auto* image : images)
                    target.blit(image, coords, program, model.fx() * size_unit, model.fy() * size_unit);
                break;
            }
            case Model::Effect::zx: {
                if(!m_zxEffect)
                    m_zxEffect = std::make_unique<ZXEffect>(m_instance);
                m_zxEffect->update(m_level.timer().tickCount());
                m_zxEffect->render(target, images[0]);
                break;
            }
        }
    }

    glDisable(GL_SCISSOR_TEST);

    if(mirror) {
        m_mirrorTarget->bind();
        FCoords topLeft = coords.in2out(mirror->fxy() * size_unit);
        FCoords size = coords.in2out_dim(mirror->size() * size_unit);
        m_mirrorTarget->blit(m_instance.graphics().offscreenTarget().texture(),
            m_instance.graphics().coords(Graphics::CoordSystems::null),
            m_instance.graphics().shaders().copy,
            0, 0, topLeft.fx() - size.fx(), topLeft.fy(), size.x(), size.y());
        target.bind();
        m_instance.graphics().setMask(m_mirrorTarget->texture().texture()); // TODO
        target.blit(mirror->anim().get(mirror->orientation())[0], coords, m_instance.graphics().shaders().mirror, mirror->fx() * size_unit, mirror->fy() * size_unit);
    }

    if(m_flashAlpha > 0) {
        glUseProgram(flatProgram);
        glUniform4fv(flatProgram.uniform("uColor"), 1, Color::white.gl(m_flashAlpha).data());
        target.fill(coords, flatProgram, 0, 0, m_winSize.fx(), m_winSize.fy());
        m_flashAlpha = std::max(m_flashAlpha - flashDecay * dt, 0.f);
    }
}

AudioData::Ref LevelScreen::addSound(const std::string &name, const std::string &filename, bool single) {
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
    if(m_music && m_music->filename() == filename)
        return;
    stopMusic();
    m_music = m_instance.audio().loadMusic(filename);
    m_instance.audio().addSource(m_music);
}

void LevelScreen::stopMusic() {
    m_instance.audio().removeSource(m_music);
    m_music.reset();
}

void LevelScreen::playSound(const std::string& name, float volume) {
    if(m_level.loading())
        return;
    auto size = m_sounds.count(name);
    auto it = m_sounds.lower_bound(name);
    std::advance(it, (int)(m_instance.rng().randomIndex(size)));
    auto& data = it->second;
    auto source = AudioSource::create(data, AudioType::sound);
    source->setVolume(volume);
    m_instance.audio().addSource(source);
}

void LevelScreen::killSounds() {
    m_instance.audio().clearExcept(m_music);
}

void LevelScreen::setShift(FCoords shift) {
    m_instance.graphics().setWindowShift(shift);
}

void LevelScreen::own_pause() {
    m_level.timer().stop();
}

void LevelScreen::own_resume() {
    m_level.timer().start();
}

bool LevelScreen::own_pointer(FCoords coords, bool longPress) {
    if(!m_level.accepting())
        return false;
    ICoords iCoords = round(coords / size_unit);
    if(longPress) {
        m_level.skipGoTo(false);
        return m_level.scheduleGoTo(iCoords);
    } else {
        // Activate fish under pointer. Actually only called after a double tap (from
        // LevelInput::doubleTap rather than LevelInput::pointerDown), based on UX feedback.
        for(int dy : {0, 1, -1}) { // allow little vertical jitter but test direct hit first
            auto* model = m_level.layout().modelAt(iCoords + ICoords{0, dy});
            if(model && model->alive())
                return m_level.rules().switchFish(model);
        }
        return false;
    }
}

bool LevelScreen::own_key(Key key) {
    switch(key) {
        case Key::up:
        case Key::down:
        case Key::left:
        case Key::right:
            if(m_level.inGoTo())
                m_level.skipGoTo(false); // break running goto commands
            if(m_level.accepting()) {
                m_level.rules().keyInput(key);
                return true;
            } else
                return false;
        case Key::space:
            if(m_level.accepting())
                m_level.rules().keyInput(key);
            else
                m_level.skipBusy();
            return true;
        case Key::interrupt:
            if(m_level.inGoTo())
                m_level.skipGoTo(false); // break running goto commands
            return true;
        case Key::skip:
            if(m_level.inGoTo())
                m_level.skipGoTo(true); // skip to final position before switching fish
            m_level.skipBusy();
            return true;
        case Key::exit:
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
        case Key::options:
            m_instance.screens().options().show();
            return true;
        default:
            return false;
    }
}

void LevelScreen::own_drawOverlays(const DrawTarget &target, float dTime, float absTime) {
    m_subs.draw(target, dTime, absTime);
    m_input.draw(target);
}

void LevelScreen::display(const std::string& filename) {
    if(!filename.empty())
        m_display = PNGImage(m_instance, filename);
    else
        m_display.reset();
}

void LevelScreen::saveEffect() {
    auto data = addSound("shutter", "sound/share/shutter.ogg");
    playSound("shutter");
    m_flashAlpha = flashInit;
}
