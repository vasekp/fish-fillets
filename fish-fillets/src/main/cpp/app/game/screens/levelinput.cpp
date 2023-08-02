#include "levelinput.h"
#include "instance.h"
#include "subsystem/graphics.h"
#include "game/screens/levelscreen.h"

LevelInput::LevelInput(Instance& instance, LevelScreen& screen) :
        m_instance(instance),
        m_screen(screen),
        m_activeFish(Model::Fish::none),
        m_dirpad({DirpadState::ignore}),
        m_buttonsFont(decoders::ttf(instance, fontFilename)),
        m_activeButton(nullptr),
        m_fishSmall(instance, "images/fishes/small/right/body_rest_00.png"),
        m_fishBig(instance, "images/fishes/big/right/body_rest_00.png")
{
    resize();
    m_buttons.push_back({ TextImage(instance, *m_buttonsFont, " "), {}, {}, Key::space });
    m_buttons.push_back({ TextImage(instance, *m_buttonsFont, "S"), {}, {}, Key::save });
    m_buttons.push_back({ TextImage(instance, *m_buttonsFont, "L"), {}, {}, Key::load });
    m_buttons.push_back({ TextImage(instance, *m_buttonsFont, "R"), {}, {}, Key::restart });
    m_buttons.push_back({ TextImage(instance, *m_buttonsFont, "O"), {}, {}, Key::options });
    m_buttons.push_back({ TextImage(instance, *m_buttonsFont, "Q"), {}, {}, Key::exit });
}

void LevelInput::setFish(Model::Fish fish) {
    m_activeFish = fish;
    if(fish == Model::Fish::none)
        m_dirpad.state = DirpadState::ignore;
    keyButton(Key::space).enabled = fish != Model::Fish::none;
}

bool LevelInput::keyDown(Key key) {
    return m_screen.keypress(key);
}

Key LevelInput::pool() {
    if(auto key = m_instance.inputSourceMasked().poolKey(); key != Key::none)
        return key;
    else if(m_dirpad.state == DirpadState::follow) {
        auto now = std::chrono::steady_clock::now();
        if(now - m_dirpad.touchTime < dirpadRepeatDelay)
            return Key::none;
        Log::debug<Log::input>("Input: sending from POOL: ", m_dirpad.lastNonzeroDir);
        return Input::toKey(m_dirpad.lastNonzeroDir);
    } else
        return Key::none;
}

bool LevelInput::pointerDown(FCoords coords) {
    if(auto button = findButton(coords); button != nullptr && button->enabled) {
        m_dirpad.state = DirpadState::button;
        m_activeButton = button;
        return true;
    }
    if(m_activeFish == Model::Fish::none || !m_screen.level().activeFishReady()) {
        m_dirpad.state = DirpadState::ignore;
        return false;
    }
    m_dirpad.touchTime = std::chrono::steady_clock::now();
    m_dirpad.history.clear();
    m_dirpad.history.emplace_front(std::chrono::steady_clock::now(), coords);
    m_dirpad.state = DirpadState::wait;
    return false;
}

bool LevelInput::pointerMove(FCoords coords) {
    if(m_dirpad.state == DirpadState::button) {
        m_activeButton = findButton(coords);
        return true;
    }
    auto now = std::chrono::steady_clock::now();
    m_dirpad.history.emplace_front(now, coords);
    auto [then, pos2] = m_dirpad.history.back();
    auto timeDiff = now - then;
    auto diff = coords - pos2;
    auto cscale = m_instance.graphics().coords(Graphics::CoordSystems::base).scale;
    bool small = diff.length() < minDistance * cscale;
    if(timeDiff > dirpadHistoryLength)
        m_dirpad.history.pop_back();
    ICoords dir{};
    if(diff.fx() > 2.f * std::abs(diff.fy()))
        dir = Direction::right;
    else if(diff.fx() < -2.f * std::abs(diff.fy()))
        dir = Direction::left;
    else if(diff.fy() > 2.f * std::abs(diff.fx()))
        dir = Direction::down;
    else if(diff.fy() < -2.f * std::abs(diff.fx()))
        dir = Direction::up;
    Log::verbose<Log::input>("move state=", (int)m_dirpad.state, " length=", diff.length(), " dir=", dir,
            " timediff=", std::chrono::duration<float>(timeDiff).count());
    switch (m_dirpad.state) {
        case DirpadState::idle:
            Log::error("Inconsistent dirpad state");
            return false;
        case DirpadState::ignore:
            return false;
        case DirpadState::goTo:
            if((coords - m_dirpad.gotoPos).length() > maxDriftGoto * cscale) {
                m_dirpad.state = DirpadState::wait;
                m_screen.keypress(Key::interrupt);
            }
            [[fallthrough]];
        case DirpadState::wait:
            if(!small && dir) {
                Log::debug<Log::input>("Input: sending from WAIT: ", dir);
                m_screen.keypress(Input::toKey(dir));
                m_dirpad.lastDir = dir;
                m_dirpad.lastNonzeroDir = dir;
                m_dirpad.touchTime = std::chrono::steady_clock::now();
                m_dirpad.state = DirpadState::follow;
                return true;
            } else
                return false;
        case DirpadState::follow:
            if(small && timeDiff > dirpadHistoryLength)
                m_dirpad.lastDir = {};
            else if(!small && dir && dir != m_dirpad.lastDir) {
                Log::debug<Log::input>("Input: sending from FOLLOW: ", dir, " (prev ", m_dirpad.lastDir, ")");
                m_screen.keypress(Input::toKey(dir));
                m_dirpad.lastNonzeroDir = m_dirpad.lastDir = dir;
            }
            return true;
        case DirpadState::button: // handled earlier
            return true;
        default:
            std::unreachable();
    }
}

bool LevelInput::pointerUp(bool empty) {
    auto lastState = m_dirpad.state;
    m_dirpad.state = DirpadState::idle;
    if(lastState == DirpadState::button && m_activeButton != nullptr) {
        if(m_activeButton->enabled)
            m_screen.keypress(m_activeButton->key);
        return true;
    }
    if(empty)
        m_screen.keypress(Key::interrupt);
    return false;
}

void LevelInput::pointerCancel() {
    m_dirpad.state = DirpadState::idle;
}

bool LevelInput::doubleTap(FCoords coords) {
    if(auto button = findButton(coords); button != nullptr && button->enabled) {
        m_dirpad.state = DirpadState::button;
        m_activeButton = button;
        return true;
    }
    auto windowCoords = m_instance.graphics().coords(Graphics::CoordSystems::window).out2in(coords);
    bool ret = false;
    if(!m_screen.doubleTap(windowCoords))
        ret = m_screen.keypress(Key::space);
    if(m_activeFish == Model::Fish::none || !m_screen.level().activeFishReady()) {
        m_dirpad.state = DirpadState::ignore;
        return ret;
    }
    m_dirpad.touchTime = std::chrono::steady_clock::now();
    m_dirpad.history.clear();
    m_dirpad.history.emplace_front(std::chrono::steady_clock::now(), coords);
    m_dirpad.state = DirpadState::wait;
    return true;
}

bool LevelInput::twoPointTap() {
    m_screen.keypress(Key::skip);
    return true;
}

bool LevelInput::longPress(FCoords coords) {
    if(m_dirpad.state == DirpadState::wait) {
        auto windowCoords = m_instance.graphics().coords(Graphics::CoordSystems::window).out2in(coords);
        bool ret = m_screen.longPress(windowCoords);
        if(ret) {
            m_dirpad.gotoPos = coords;
            m_dirpad.state = DirpadState::goTo;
        }
        return ret;
    } else
        return false;
}

void LevelInput::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::buttons);
    m_buttonsFont->setSizes(buttonFontSize, 0.f, coords.scale);
    {
        auto buttonCount = m_buttons.size();
        FCoords extent = {buttonSize, buttonSize};
        for(auto i = 0u; i < buttonCount; i++) {
            FCoords center = (buttonSize + buttonDistance) * ((float)i - (float)(buttonCount - 1) / 2.f) * coords.principal;
            m_buttons[i].coordsFrom = center - extent / 2.f;
            m_buttons[i].coordsTo = center + extent / 2.f;
            m_buttons[i].image.render();
        }
    }
}

void LevelInput::setSavePossible(bool possible) {
    keyButton(Key::save).enabled = possible;
}

void LevelInput::setLoadPossible(bool possible) {
    keyButton(Key::load).enabled = possible;
}

void LevelInput::setRestartPossible(bool possible) {
    keyButton(Key::restart).enabled = possible;
}

void LevelInput::flashButton(Key which) {
    keyButton(which).flashing = true;
    keyButton(which).flashTime = 0.f;
}

void LevelInput::update(float time) {
    // update buttons
    for(auto& button : m_buttons) {
        if(button.flashing) {
            if(!button.flashTime)
                button.flashTime = time;
            else if(button.flashTime + flashDuration < time)
                button.flashing = false;
        }
        if(button.flashing && std::fmod(time - button.flashTime, flashDuration * 2 / 5) < flashDuration / 5)
            button.alpha = alphaFlash;
        else if(!button.enabled)
            button.alpha = alphaDisabled;
        else if(m_dirpad.state == DirpadState::button)
            button.alpha = &button == m_activeButton ? alphaActive : alphaBase;
        else
            button.alpha = alphaBase;
    }
}

void LevelInput::draw(DrawTarget& target) {
    drawButtons(target);
    drawDirpad(target);
}

void LevelInput::drawButtons(DrawTarget& target) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::buttons);
    for(const auto& button : m_buttons) {
        const auto program = m_instance.graphics().shaders().button({ .color = colorButtons.gl(button.alpha) });
        target.draw(&button.image, program, coords, { .dest = button.coordsFrom, .area = button.coordsTo - button.coordsFrom });
    }
    if(m_activeFish != Model::Fish::none) {
        const auto& button = keyButton(Key::space);
        const auto& image = m_activeFish == Model::Fish::small ? m_fishSmall : m_fishBig;
        const auto alphaProgram = m_instance.graphics().shaders().alpha({
            .alpha = m_screen.level().activeFishReady() ? 1.f : 0.5f
        });
        FCoords center = (button.coordsFrom + button.coordsTo) / 2.f;
        FCoords extent = 0.8f * (button.coordsTo - button.coordsFrom);
        FCoords imgSize = image.size();
        float scale = std::min(extent.fx() / imgSize.fx(), extent.fy() / imgSize.fy());
        FCoords from = center - scale / 2.f * imgSize;
        FCoords to = center + scale / 2.f * imgSize;
        target.draw(&image, alphaProgram, coords, { .srcSize = to - from, .dest = from });
    }
}

void LevelInput::drawDirpad(DrawTarget& target) {
    if(m_dirpad.state != DirpadState::wait && m_dirpad.state != DirpadState::follow && m_dirpad.state != DirpadState::goTo)
        return;

    float baseAlpha;
    if(m_dirpad.state == DirpadState::wait && m_dirpad.touchTime != absolutePast) {
        std::chrono::duration<float> dt = std::chrono::steady_clock::now() - m_dirpad.touchTime;
        float q = std::min(dt / dirpadAppearTime, 1.f);
        baseAlpha = 3*q*q - 2*q*q*q;
    } else
        baseAlpha = 1;

    auto& graphics = m_instance.graphics();
    const auto& coords = graphics.coords(Graphics::CoordSystems::buttons);
    bool inwards = m_dirpad.state == DirpadState::goTo;
    auto program = graphics.shaders().arrow({
        .position = m_dirpad.state == DirpadState::goTo ? m_dirpad.gotoPos : m_dirpad.history.front().second,
        .size = arrowSize * coords.scale,
        .sign = inwards ? -1.f : 1.f
    });
    auto color = m_activeFish == Model::Fish::small ? colorSmall : colorBig;

    for([[maybe_unused]] auto dir : {Direction::up, Direction::down, Direction::left, Direction::right}) {
        program.params().direction = FCoords{dir};
        bool active = (m_dirpad.state == DirpadState::follow && dir == m_dirpad.lastNonzeroDir) || m_dirpad.state == DirpadState::goTo;
        float alpha = (active ? alphaActive : alphaBase) * baseAlpha;
        program.params().color = color.gl(alpha);
        target.draw(BaseProgram::Shape::triangle, program, coords);
    }
}

const LevelInput::Button* LevelInput::findButton(FCoords pos) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::buttons);
    auto pos1 = coords.out2in(pos);
    for(const auto& button : m_buttons)
        if(pos1.within(button.coordsFrom, button.coordsTo))
            return &button;
    return nullptr;
}

LevelInput::Button& LevelInput::keyButton(Key key) {
    auto it = std::find_if(m_buttons.begin(), m_buttons.end(), [key](const Button& b) -> bool { return b.key == key; });
    assert(it != m_buttons.end());
    return *it;
}
