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
        m_activeButton(noButton)
{
    std::fill(m_buttonsEnabled.begin(),  m_buttonsEnabled.end(), true);
}

void LevelInput::setFish(Model::Fish fish) {
    m_activeFish = fish;
    if(fish == Model::Fish::none)
        m_dirpad.state = DirpadState::ignore;
}

bool LevelInput::keyDown(Key key) {
    return m_screen.keypress(key);
}

bool LevelInput::keyUp(Key) {
    return false;
}

Key LevelInput::pool() {
    if(auto key = m_instance.inputSource().poolKey(); key != Key::none)
        return key;
    else if(m_dirpad.state == DirpadState::follow) {
        Log::verbose("Input: sending from POLL: ", m_dirpad.lastNonzeroDir);
        return Input::toKey(m_dirpad.lastNonzeroDir);
    } else
        return Key::none;
}

bool LevelInput::pointerDown(FCoords coords) {
    if(auto button = findButton(coords); button != noButton && m_buttonsEnabled[button]) {
        m_dirpad.state = DirpadState::button;
        m_activeButton = button;
        return true;
    }

    if(m_activeFish == Model::Fish::none) {
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
    Log::verbose("move state=", (int)m_dirpad.state, " length=", diff.length(), " dir=", dir,
            " timediff=", std::chrono::duration<float>(timeDiff).count());
    switch (m_dirpad.state) {
        case DirpadState::idle:
            Log::error("Inconsistent dirpad state");
            return false;
        case DirpadState::ignore:
            return false;
        case DirpadState::wait:
            if(!small && dir) {
                Log::verbose("Input: sending from WAIT: ", dir);
                m_screen.keypress(Input::toKey(dir));
                m_dirpad.lastDir = dir;
                m_dirpad.lastNonzeroDir = dir;
                m_dirpad.state = DirpadState::follow;
                return true;
            } else
                return false;
        case DirpadState::follow:
            if(small)
                m_dirpad.lastDir = {};
            else if(dir && dir != m_dirpad.lastDir) {
                Log::verbose("Input: sending from FOLLOW: ", dir, " (prev ", m_dirpad.lastDir, ")");
                m_screen.keypress(Input::toKey(dir));
                m_dirpad.lastNonzeroDir = m_dirpad.lastDir = dir;
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
        if(m_buttonsEnabled[m_activeButton])
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
    auto windowCoords = m_instance.graphics().coords(Graphics::CoordSystems::window).out2in(coords);
    if(!m_screen.pointer(windowCoords))
        m_screen.keypress(Key::space);
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
        return m_screen.pointer(windowCoords, true);
    } else
        return false;
}

void LevelInput::refresh() {
    auto& graphics = m_instance.graphics();
    auto displayWidth = (float)graphics.display().width();
    auto displayHeight = (float)graphics.display().height();
    auto coords = graphics.coords(Graphics::CoordSystems::buttons);
    {
        auto& program = graphics.shaders().arrow;
        glUseProgram(program);
        glUniform1f(program.uniform("uSize"), arrowSize * coords.scale);
        glUniform2f(program.uniform("uDstSize"), displayWidth, displayHeight);
    }
    {
        constexpr auto buttonCount = bSIZE;
        float buttonSizeScaled = buttonSize * coords.scale;

        std::array<std::string, buttonCount> chars{"S", "L", "R", "O", "Q"};
        std::array<Key, buttonCount> keys{Key::save, Key::load, Key::restart, Key::options, Key::exit};
        static_assert(std::tuple_size_v<decltype(chars)> == buttonCount);
        static_assert(std::tuple_size_v<decltype(keys)> == buttonCount);
        m_buttonsFont->setSizes(buttonSizeScaled, 0);
        FCoords extent = {buttonSize, buttonSize};
        for(auto i = 0u; i < buttonCount; i++) {
            FCoords center = (buttonSize + buttonDistance) * ((float)i - (float)(buttonCount - 1) / 2.f) * coords.principal;
            m_buttons[i] = {
                    m_buttonsFont->renderText(chars[i]),
                    center - extent / 2.f,
                    center + extent / 2.f,
                    keys[i]
            };
        }
    }
}

void LevelInput::setSavePossible(bool possible) {
    m_buttonsEnabled[Buttons::bSave] = possible;
}

void LevelInput::setLoadPossible(bool possible) {
    m_buttonsEnabled[Buttons::bLoad] = possible;
}

void LevelInput::draw(const DrawTarget& target) {
    drawButtons(target);
    drawDirpad(target);
}

void LevelInput::drawButtons(const DrawTarget& target) {
    auto& program = m_instance.graphics().shaders().button;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::buttons);
    glUseProgram(program);
    for(auto i = 0u; i < m_buttons.size(); i++) {
        float alpha = !m_buttonsEnabled[i] ? .25f
                : m_dirpad.state == DirpadState::button && (int)i == m_activeButton
                    ? 1.0f
                    : 0.5f;
        glUniform4fv(program.uniform("uColor"), 1, colorButtons.gl(alpha).data());
        m_buttons[i].texture.bind();
        target.fill(coords, program, m_buttons[i].coordsFrom.fx(), m_buttons[i].coordsFrom.fy(), m_buttons[i].coordsTo.fx(), m_buttons[i].coordsTo.fy());
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

    auto& program = m_instance.graphics().shaders().arrow;
    glUseProgram(program);

    constexpr float coords[3][3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };
    glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &coords[0][0]);

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
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::buttons);
    auto pos0 = coords.out2in(pos);
    for(auto i = 0u; i < m_buttons.size(); i++) {
        auto& button = m_buttons[i];
        if(pos0.within(button.coordsFrom, button.coordsTo))
            return (int)i;
    }
    return noButton;
}