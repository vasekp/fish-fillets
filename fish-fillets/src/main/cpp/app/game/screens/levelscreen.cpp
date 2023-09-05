#include "levelscreen.h"
#include "game/logic/level.h"
#include "game/logic/layout.h"
#include "game/logic/rules.h"
#include "screenmanager.h"
#include "subsystem/rng.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"

static constexpr float flashInit = 0.5f;
static constexpr auto flashDecay = 500ms;
static constexpr Color ropeColor{0x30404E};
static constexpr int size_unit = 15;

LevelScreen::LevelScreen(Instance& instance, LevelRecord& record, bool replay) :
        GameScreen(instance),
        m_level(instance, *this, record),
        m_input(instance, *this),
        m_waves(),
        m_winSize(),
        m_subs(instance),
        m_lastUpdate(),
        m_flashAlpha(0),
        m_replay(replay)
{ }

LevelScreen::~LevelScreen() = default;

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
    m_level.init();
    if(m_replay)
        m_level.replay();
}

void LevelScreen::own_resize() {
    m_winSize = FCoords{m_level.layout().size()} * size_unit;
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

std::unique_ptr<TextureTarget> LevelScreen::makeMirrorTarget(const Model& model) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    auto modelSizePixel = coords.in2out_dim(FCoords{model.size()} * size_unit).toSize();
    return std::make_unique<TextureTarget>(m_instance.graphics().backend(), modelSizePixel);
}

void LevelScreen::own_update() {
    auto time = liveTime();
    auto dt = time - m_lastUpdate;

    m_level.update(dt);

    if(m_flashAlpha > 0)
        m_flashAlpha = std::max(m_flashAlpha - dt / flashDecay * flashInit, 0.f);

    m_subs.update(time);
    m_lastUpdate = time;
}

void LevelScreen::own_draw(DrawTarget& target) {
    drawLevel(target);

    if(m_flashAlpha > 0) {
        const auto program = m_instance.graphics().shaders().flat({ .color = Color::white.gl(m_flashAlpha) });
        const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
        target.draw(program, coords, { .area = m_winSize });
    }

    m_subs.draw(target, liveTime());
    if(m_hint)
        m_hint->draw(target);
    m_input.draw(target);
}

void LevelScreen::drawLevel(DrawTarget& target) {
    const auto copyProgram = m_instance.graphics().shaders().copy();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);

    if(m_display) {
        target.draw(m_display.value(), copyProgram, coords);
        return;
    }

    m_instance.graphics().setScissor(coords.in2out({0, 0}).round(), coords.in2out(m_winSize).round());

    auto [amplitude, period, speed] = m_waves;

    float phase = std::fmod(timeAlive() * speed * 10.f, 2.f * std::numbers::pi_v<float>);
    const auto wavyProgram = m_instance.graphics().shaders().wavyImage({
        .amplitude = amplitude,
        .period = period,
        .phase = phase
    });

    target.draw(getImage("background"), wavyProgram, coords);

    const auto ropeProgram = m_instance.graphics().shaders().flat({ .color = ropeColor.gl() });
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
        const auto [imgMain, imgOverlay] = model.anim().get(model.orientation());
        switch(effect) {
            case Model::Effect::none:
                target.draw(imgMain, copyProgram, coords, { .dest = model.fxy() * size_unit });
                if(imgOverlay) {
                    const auto overlayProgram = m_instance.graphics().shaders().overlay();
                    target.draw(*imgOverlay, overlayProgram, coords, { .dest = model.fxy() * size_unit });
                }
                break;
            case Model::Effect::invisible:
                std::unreachable();
            case Model::Effect::mirror:
                mirror = &model;
                break;
            case Model::Effect::reverse: {
                const auto program = m_instance.graphics().shaders().reverse();
                assert(!imgOverlay);
                target.draw(imgMain, program, coords, { .dest = model.fxy() * size_unit });
                break;
            }
            case Model::Effect::disintegrate: {
                const auto program = m_instance.graphics().shaders().disintegrate({ .time = timeAlive() - effectTime });
                assert(!imgOverlay);
                target.draw(imgMain, program, coords, { .dest = model.fxy() * size_unit });
                break;
            }
            case Model::Effect::zx: {
                if(!m_zxEffect)
                    m_zxEffect = std::make_unique<ZXEffect>(m_instance);
                m_zxEffect->update(m_level.timer().tickCount());
                assert(!imgOverlay);
                m_zxEffect->render(target, imgMain);
                break;
            }
        }
    }

    m_instance.graphics().releaseScissor();

    if(mirror) {
        FCoords topLeft = coords.in2out(mirror->fxy() * size_unit);
        FCoords size = coords.in2out_dim(FCoords{mirror->size()} * size_unit);
        m_mirrorTarget->draw(m_instance.graphics().offscreenTarget().texture(),
            copyProgram, m_instance.graphics().coords(Graphics::CoordSystems::null),
            {
                .src = FCoords{topLeft.x - size.x, topLeft.y},
                .area = size
            });
        const auto program = m_instance.graphics().shaders().mirror();
        const auto* image = mirror->anim().get(mirror->orientation()).first;
        target.draw({
                image->texture(),
                m_mirrorTarget->texture()
            }, program, coords, { .dest = mirror->fxy() * size_unit });
    }
}

AudioData::Ref LevelScreen::addSound(const std::string& name, const std::string& filename, bool single) {
    if(single && m_sounds.contains(name))
        return m_sounds.find(name)->second;
    auto it = m_sounds.insert({name, m_instance.audio().loadSound(filename)});
    return it->second;
}

void LevelScreen::addSubtitle(const std::string& text, const std::string& colors) {
    m_subs.add(text, colors);
}

void LevelScreen::showHint(const std::string& text) {
    m_hint.emplace(m_instance, text, false);
}

void LevelScreen::hideHint() {
    m_hint.reset();
}

void LevelScreen::setWaves(float amplitude, float period, float speed) {
    m_waves = {amplitude, period, speed};
}

void LevelScreen::playMusic(const std::string& filename) {
    if(m_music && m_music->name() == filename)
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
    std::advance(it, m_instance.rng().randomIndex(size));
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

bool LevelScreen::input_goTo(FCoords coords) {
    if(!m_level.accepting())
        return false;
    ICoords iCoords = round(coords / size_unit);
    m_level.skipGoTo(false);
    return m_level.enqueueGoTo(iCoords);
}

bool LevelScreen::input_switchFish(FCoords coords) {
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

void LevelScreen::display(ImageRef&& image) {
    m_display = std::move(image);
}

void LevelScreen::display() {
    m_display = {};
}

Image* LevelScreen::displayed() {
    return m_display.value();
}

void LevelScreen::saveEffect() {
    addSound("shutter", "sound/share/shutter.ogg");
    playSound("shutter");
    m_flashAlpha = flashInit;
}
