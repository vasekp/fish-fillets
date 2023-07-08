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
        m_fishSmall(instance, "images/fishes/small/right/body_rest_00.png"),
        m_fishBig(instance, "images/fishes/big/right/body_rest_00.png"),
        m_activeButton(noButton)
{
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
        Log::verbose<Log::input>("Input: sending from POOL: ", m_dirpad.lastNonzeroDir);
        return Input::toKey(m_dirpad.lastNonzeroDir);
    } else
        return Key::none;
}

bool LevelInput::pointerDown(FCoords coords) {
    if(auto button = findButton(coords); button != noButton && m_buttons[button].enabled) {
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
    m_dirpad.inside = false;
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
    bool small = diff.length() < minDistance * m_instance.graphics().coords(Graphics::CoordSystems::base).scale;
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
        case DirpadState::wait:
            if(!small && dir) {
                Log::verbose<Log::input>("Input: sending from WAIT: ", dir);
                m_screen.keypress(Input::toKey(dir));
                m_dirpad.lastDir = dir;
                m_dirpad.lastNonzeroDir = dir;
                m_dirpad.state = DirpadState::follow;
                m_dirpad.inside = false;
                return true;
            } else
                return false;
        case DirpadState::follow:
            if(small)
                m_dirpad.lastDir = {};
            else if(dir && dir != m_dirpad.lastDir) {
                Log::verbose<Log::input>("Input: sending from FOLLOW: ", dir, " (prev ", m_dirpad.lastDir, ")");
                m_screen.keypress(Input::toKey(dir));
                m_dirpad.lastNonzeroDir = m_dirpad.lastDir = dir;
                m_dirpad.inside = false;
            }
            return true;
        case DirpadState::button: // handled earlier
            return true;
    }
    std::unreachable();
}

bool LevelInput::pointerUp(bool empty) {
    auto lastState = m_dirpad.state;
    m_dirpad.state = DirpadState::idle;
    if(lastState == DirpadState::button && m_activeButton != noButton) {
        if(m_buttons[m_activeButton].enabled)
            m_screen.keypress(m_buttons[m_activeButton].key);
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
    if(auto button = findButton(coords); button != noButton && m_buttons[button].enabled) {
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
        if(ret)
            m_dirpad.inside = true;
        return ret;
    } else
        return false;
}

void LevelInput::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::buttons);
    {
        auto buttonCount = m_buttons.size();
        m_buttonsFont->setSizes(buttonFontSize * coords.scale, 0);
        FCoords extent = {buttonSize, buttonSize};
        for(auto i = 0u; i < buttonCount; i++) {
            FCoords center = (buttonSize + buttonDistance) * ((float)i - (float)(buttonCount - 1) / 2.f) * coords.principal;
            m_buttons[i].coordsFrom = coords.in2out(center - extent / 2.f);
            m_buttons[i].coordsTo = coords.in2out(center + extent / 2.f);
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
}

void LevelInput::draw(const DrawTarget& target, float time) {
    drawButtons(target, time);
    drawDirpad(target);
}

void LevelInput::drawButtons(const DrawTarget& target, float time) {
    auto& program = m_instance.graphics().shaders().button;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::null);
    glUseProgram(program);
    for(auto i = 0u; i < m_buttons.size(); i++) {
        auto& button = m_buttons[i];
        if(button.flashing) {
            if(!button.flashTime)
                button.flashTime = time;
            else if(button.flashTime + flashDuration < time) {
                button.flashing = false;
                button.flashTime = 0.f;
            }
        }
        float alpha = button.flashing && std::fmod(time - button.flashTime, flashDuration * 2 / 5) < flashDuration / 5
                ? flashBrightness
                : !button.enabled
                    ? 0.25f
                    : m_dirpad.state == DirpadState::button && (int)i == m_activeButton
                        ? 1.0f
                        : 0.5f;
        glUniform4fv(program.uniform("uColor"), 1, colorButtons.gl(alpha).data());
        glUniform2f(program.uniform("uTexSize"), (float)button.image.width(), (float)button.image.height());
        button.image.texture().bind();
        target.fill(coords, program, button.coordsFrom.fx(), button.coordsFrom.fy(), button.coordsTo.fx(), button.coordsTo.fy());
    }
    if(m_activeFish != Model::Fish::none) {
        auto& alphaProgram = m_instance.graphics().shaders().alpha;
        auto& button = keyButton(Key::space);
        auto& image = m_activeFish == Model::Fish::small ? m_fishSmall : m_fishBig;
        glUseProgram(alphaProgram);
        glUniform1f(alphaProgram.uniform("uAlpha"), m_screen.level().activeFishReady() ? 1.f : 0.5f);
        FCoords center = (button.coordsFrom + button.coordsTo) / 2.f;
        FCoords extent = 0.8f * (button.coordsTo - button.coordsFrom);
        FCoords imgExtent(image.width(), image.height());
        float scale = std::min(extent.fx() / imgExtent.fx(), extent.fy() / imgExtent.fy());
        FCoords from = center - scale / 2.f * imgExtent;
        FCoords to = center + scale / 2.f * imgExtent;
        image.texture().bind();
        target.fill(coords, alphaProgram, from.fx(), from.fy(), to.fx(), to.fy());
    }
}

void LevelInput::drawDirpad(const DrawTarget& target) {
    if(m_dirpad.state != DirpadState::wait && m_dirpad.state != DirpadState::follow)
        return;

    float baseAlpha;
    if(m_dirpad.touchTime != absolutePast) {
        std::chrono::duration<float> dt = std::chrono::steady_clock::now() - m_dirpad.touchTime;
        float q = std::min(dt / dirpadAppearTime, 1.f);
        baseAlpha = 3*q*q - 2*q*q*q;
    } else
        baseAlpha = 1;

    auto& graphics = m_instance.graphics();
    auto& program = graphics.shaders().arrow;
    const auto& coords = graphics.coords(Graphics::CoordSystems::buttons);
    glUseProgram(program);

    float s = m_dirpad.inside ? -1 : 1; // TODO uniform bool?
    float vertices[3][3] = {
        {s, 0, 0},
        {0, s, 0},
        {0, 0, s}
    };
    glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &vertices[0][0]);

    auto displayDim = graphics.display().size();
    glUniform1f(program.uniform("uSize"), arrowSize * coords.scale);
    glUniform2f(program.uniform("uDstSize"), displayDim.fx(), displayDim.fy());
    auto [_, pos] = m_dirpad.history.front();
    glUniform2f(program.uniform("uPosition"), pos.fx(), pos.fy());
    auto color = m_activeFish == Model::Fish::small ? colorSmall : colorBig;

    for(auto dir : {Direction::up, Direction::down, Direction::left, Direction::right}) {
        glUniform2fv(program.uniform("uDirection"), 1, FCoords{dir}.gl().data());
        float alpha = (m_dirpad.state == DirpadState::follow && dir == m_dirpad.lastNonzeroDir ? 1.f : 0.5f) * baseAlpha;
        glUniform4fv(program.uniform("uColor"), 1, color.gl(alpha).data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

int LevelInput::findButton(FCoords pos) {
    for(auto i = 0u; i < m_buttons.size(); i++) {
        auto& button = m_buttons[i];
        if(pos.within(button.coordsFrom, button.coordsTo))
            return (int)i;
    }
    return noButton;
}

LevelInput::Button& LevelInput::keyButton(Key key) {
    auto it = std::find_if(m_buttons.begin(), m_buttons.end(), [key](const Button& b) -> bool { return b.key == key; });
    assert(it != m_buttons.end());
    return *it;
}
