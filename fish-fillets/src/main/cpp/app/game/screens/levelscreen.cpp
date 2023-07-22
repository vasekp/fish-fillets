#include "levelscreen.h"
#include "game/logic/level.h"
#include "screenmanager.h"
#include "subsystem/rng.h"

LevelScreen::LevelScreen(Instance& instance, LevelRecord& record, bool replay) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_input(instance, *this),
        m_waves(),
        m_winSize(),
        m_subs(instance),
        m_lastUpdate(0),
        m_flashAlpha(0),
        m_replay(replay)
{ }

void LevelScreen::reset() {
    m_display.reset();
    m_instance.graphics().setWindowShift({});
    subs().clear();
    m_hint.reset();
}

void LevelScreen::exit() {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
}

void LevelScreen::own_start() {
    m_instance.audio().clear();
    m_level.init();
    if(m_replay)
        m_level.replay();
}

void LevelScreen::own_resize() {
    m_winSize = FCoords{m_level.layout().width(), m_level.layout().height()} * size_unit;
    m_instance.graphics().setWindowSize(m_winSize);
    m_input.resize();
    m_subs.resize();
    if(m_hint)
        m_hint->resize();

    const auto& models = m_level.layout().models();
    auto it = std::find_if(models.begin(), models.end(), [&](const auto& model) { return model->effect().name == Model::Effect::mirror; });
    if(it == models.end())
        m_mirrorTarget = {};
    else
        m_mirrorTarget = makeMirrorTarget(**it);
}

std::unique_ptr<TextureTarget> LevelScreen::makeMirrorTarget(const Model &model) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    FCoords modelSizePixel = coords.in2out_dim(model.size() * size_unit);
    auto ret = std::make_unique<TextureTarget>(m_instance.graphics().system());
    ret->resize(modelSizePixel.x(), modelSizePixel.y());
    return ret;
}

void LevelScreen::own_update() {
    auto liveTime = timeAlive();
    auto dt = liveTime - m_lastUpdate;

    m_level.update(dt);

    if(m_flashAlpha > 0)
        m_flashAlpha = std::max(m_flashAlpha - flashDecay * dt, 0.f);

    m_subs.update(liveTime, dt);
    m_input.update(liveTime);

    m_lastUpdate = liveTime;
}

void LevelScreen::own_draw(DrawTarget& target) {
    drawLevel(target);

    if(m_flashAlpha > 0) {
        const auto program = m_instance.graphics().shaders().flat({ .color = Color::white, .alpha = m_flashAlpha });
        const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
        target.draw(program, coords, { .area = m_winSize });
    }

    m_subs.draw(target, timeAlive());
    if(m_hint)
        m_hint->draw(target);
    m_input.draw(target);
}

void LevelScreen::drawLevel(DrawTarget& target) {
    const auto copyProgram = m_instance.graphics().shaders().copy();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);

    if(m_display) {
        target.draw(&m_display.value(), copyProgram, coords);
        return;
    }

    m_instance.graphics().system().setScissor(coords.in2out({0, 0}).round(), coords.in2out(m_winSize).round());

    float phase = std::fmod(timeAlive(), (float)(2 * M_PI));
    const auto wavyProgram = m_instance.graphics().shaders().wavyImage({
        .amplitude = m_waves[0],
        .period = m_waves[1],
        .speed = m_waves[2],
        .phase = phase
    });

    target.draw(getImage("background"), wavyProgram, coords);

    const auto ropeProgram = m_instance.graphics().shaders().flat({ .color = ropeColor });
    for(const auto& rope : m_level.layout().getRopes()) {
        FCoords c1 = rope.m1->fxy() * size_unit + rope.d1;
        FCoords c2 = rope.m2->fxy() * size_unit + rope.d2;
        target.draw(ropeProgram, coords, { .dest = c2, .area = c1 - c2 + FCoords{1, 0} });
    }

    const Model* mirror = nullptr;
    for(const auto& uModel : m_level.layout().models()) {
        const auto& model = *uModel;
        if(model.hidden())
            continue;
        auto [effect, effectTime] = model.effect();
        if(effect == Model::Effect::invisible)
            continue;
        const auto images = model.anim().get(model.orientation());
        switch(effect) {
            case Model::Effect::none:
                for(const auto* image : images)
                    target.draw(image, copyProgram, coords, { .dest = model.fxy() * size_unit });
                break;
            case Model::Effect::invisible:
                std::unreachable();
            case Model::Effect::mirror:
                mirror = &model;
                break;
            case Model::Effect::reverse: {
                const auto program = m_instance.graphics().shaders().reverse();
                for(const auto* image : images)
                    target.draw(image, program, coords, { .dest = model.fxy() * size_unit });
                }
                break;
            case Model::Effect::disintegrate: {
                const auto program = m_instance.graphics().shaders().disintegrate({ .time = timeAlive() - effectTime });
                for(const auto* image : images)
                    target.draw(image, program, coords, { .dest = model.fxy() * size_unit });
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

    m_instance.graphics().system().releaseScissor();

    if(mirror) {
        FCoords topLeft = coords.in2out(mirror->fxy() * size_unit);
        FCoords size = coords.in2out_dim(mirror->size() * size_unit);
        m_mirrorTarget->draw(m_instance.graphics().offscreenTarget().texture(),
            copyProgram, m_instance.graphics().coords(Graphics::CoordSystems::null),
            {
                .src = FCoords{topLeft.fx() - size.fx(), topLeft.fy()},
                .area = size
            });
        const auto program = m_instance.graphics().shaders().mirror({
            .maskImage = m_mirrorTarget->texture()
        });
        target.draw(mirror->anim().get(mirror->orientation())[0],
            program, coords, { .dest = mirror->fxy() * size_unit });
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

void LevelScreen::showHint(const std::string &text) {
    m_hint.emplace(m_instance, text);
}

void LevelScreen::hideHint() {
    m_hint.reset();
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

bool LevelScreen::longPress(FCoords coords) { // TODO rename semantically
    if(!m_level.accepting())
        return false;
    ICoords iCoords = round(coords / size_unit);
    m_level.skipGoTo(false);
    return m_level.enqueueGoTo(iCoords);
}

bool LevelScreen::doubleTap(FCoords coords) { // TODO rename semantically
    if(!m_level.accepting())
        return false;
    // Try find and activate fish under pointer.
    ICoords iCoords = round(coords / size_unit);
    for(int dy : {0, 1, -1}) { // allow little vertical jitter but test direct hit first
        auto* model = m_level.layout().modelAt(iCoords + ICoords{0, dy});
        if(model && model->alive())
            return m_level.rules().switchFish(model);
    }
    // If not found, leave to LevelInput.
    return false;
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
            if(m_level.inSlideshow())
                m_level.quitSlideshow();
            else
                exit();
            return true;
        case Key::save:
            if(m_level.savePossible()) {
                m_level.save();
                return true;
            } else
                return false;
        case Key::load:
            if(m_level.loadPossible()) {
                m_level.load();
                return true;
            } else return false;
        case Key::restart:
            if(!m_level.inReplay()) {
                m_level.restart();
                return true;
            } else return false;
        case Key::options:
            m_instance.screens().options().show();
            return true;
        default:
            return false;
    }
}

void LevelScreen::display(const std::string& filename) {
    if(!filename.empty())
        m_display = PNGImage(m_instance, filename);
    else
        m_display.reset();
}

void LevelScreen::saveEffect() {
    addSound("shutter", "sound/share/shutter.ogg");
    playSound("shutter");
    m_flashAlpha = flashInit;
}
