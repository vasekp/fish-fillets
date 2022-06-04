#include "levelinput.h"
#include "instance.h"
#include "game/screens/screenmanager.h"
#include "subsystem/graphics.h"

LevelInput::LevelInput(Instance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_dirpad({DirpadState::ignore}),
        m_gravity(ButtonGravity::left),
        m_activeButton(noButton)
{
    std::fill(m_buttonsEnabled.begin(),  m_buttonsEnabled.end(), true);
}

void LevelInput::setFish(Model::Fish fish) {
    m_dirpad.fish = fish;
    if(fish == Model::Fish::none)
        m_dirpad.state = DirpadState::ignore;
}

unsigned LevelInput::index(Key key) {
    return (unsigned)key;
}

bool LevelInput::handleKeyDown(Key key) {
    m_lastKey = key;
    return m_instance.screens().dispatchKey(key);
}

bool LevelInput::handleKeyUp(Key) {
    m_lastKey = Key::none;
    return false;
}

Key LevelInput::pool() {
    if(m_lastKey != Key::none)
        return m_lastKey;
    else if(m_dirpad.state == DirpadState::follow) {
        Log::verbose("Input: sending from POLL: ", m_dirpad.lastNonzeroDir);
        return Input::toKey(m_dirpad.lastNonzeroDir);
    } else
        return Key::none;
}

bool LevelInput::handlePointerDown(FCoords pos) {
    if(auto button = findButton(pos); button != noButton && m_buttonsEnabled[button]) {
        m_dirpad.state = DirpadState::button;
        m_activeButton = button;
        return true;
    }

    bool handled = false;
    if(auto windowCoords = m_instance.graphics().windowTarget().screen2window(pos); m_instance.screens().dispatchPointer(windowCoords)) {
        m_dirpad.touchTime = absolutePast;
        handled = true;
    } else if(std::chrono::steady_clock::now() < m_dirpad.touchTime + doubletapTime) {
        m_instance.screens().dispatchKey(Key::space);
        m_dirpad.touchTime = absolutePast;
        handled = true;
    } else
        m_dirpad.touchTime = std::chrono::steady_clock::now();
    if(m_dirpad.fish == Model::Fish::none) {
        m_dirpad.state = DirpadState::ignore;
        return handled;
    }
    m_dirpad.history.clear();
    m_dirpad.history.emplace_front(std::chrono::steady_clock::now(), pos);
    m_dirpad.state = DirpadState::wait;
    return handled;
}

bool LevelInput::handlePointerMove(FCoords pos) {
    if(m_dirpad.state == DirpadState::button) {
        m_activeButton = findButton(pos);
        return true;
    }
    auto now = std::chrono::steady_clock::now();
    m_dirpad.history.emplace_front(now, pos);
    auto [then, pos2] = m_dirpad.history.back();
    auto timeDiff = now - then;
    auto diff = pos - pos2;
    bool small = diff.length() < minDistance * m_instance.graphics().dpi();
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
                m_instance.screens().dispatchKey(Input::toKey(dir));
                m_dirpad.lastDir = dir;
                m_dirpad.lastNonzeroDir = dir;
                m_dirpad.touchTime = absolutePast;
                m_dirpad.state = DirpadState::follow;
                return true;
            } else
                return false;
        case DirpadState::follow:
            if(small)
                m_dirpad.lastDir = {};
            else if(dir && dir != m_dirpad.lastDir) {
                Log::verbose("Input: sending from FOLLOW: ", dir, " (prev ", m_dirpad.lastDir, ")");
                m_instance.screens().dispatchKey(Input::toKey(dir));
                m_dirpad.lastNonzeroDir = m_dirpad.lastDir = dir;
            }
            return true;
        case DirpadState::button: // handled earlier
            return true;
    }
}

bool LevelInput::handlePointerUp() {
    if(m_dirpad.state == DirpadState::button && m_activeButton != noButton) {
        if(m_buttonsEnabled[m_activeButton])
            m_instance.screens().dispatchKey(m_buttons[m_activeButton].key);
    }
    m_dirpad.state = DirpadState::idle;
    return false;
}

void LevelInput::checkLongPress() {
    if(m_dirpad.state == DirpadState::wait
            && m_dirpad.touchTime != absolutePast
            && std::chrono::steady_clock::now() > m_dirpad.touchTime + longpressTime) {
        if(auto windowCoords = m_instance.graphics().windowTarget().screen2window(m_dirpad.history.front().second);
                m_instance.screens().dispatchPointer(windowCoords, true))
            m_dirpad.state = DirpadState::ignore; // regardless of success
        m_dirpad.touchTime = absolutePast;
    }
}

void LevelInput::refresh() {
    auto displayWidth = (float) m_instance.graphics().display().width();
    auto displayHeight = (float) m_instance.graphics().display().height();
    {
        auto& program = m_instance.graphics().shaders().arrow;
        glUseProgram(program);
        glUniform1f(program.uniform("uSize"), arrowSize * m_instance.graphics().dpi());
        glUniform2f(program.uniform("uDstSize"), displayWidth, displayHeight);
    }
    {
        auto& program = m_instance.graphics().shaders().button;
        glUseProgram(program);
        glUniform2f(program.uniform("uDstSize"), displayWidth, displayHeight);

        constexpr auto buttonCount = std::tuple_size_v<decltype(m_buttons)>;
        float buttonSize = std::min(maxButtonSize * m_instance.graphics().dpi(), displayHeight / (float)buttonCount);
        float fullSize = m_gravity == ButtonGravity::left ? displayHeight : displayWidth;
        float buttonStride = std::min((fullSize - (float)buttonCount * buttonSize) / (float)(buttonCount - 1), maxButtonGap * m_instance.graphics().dpi()) + buttonSize;
        float buttonOffset = (fullSize - (buttonCount - 1) * buttonStride - buttonSize) / 2.f;
        glUniform1f(program.uniform("uSize"), buttonSize);

        std::array<std::string, buttonCount> chars{"S", "L", "R", "O", "Q"};
        std::array<Key, buttonCount> keys{Key::save, Key::load, Key::restart, Key::options, Key::exit};
        static_assert(std::tuple_size_v<decltype(chars)> == std::tuple_size_v<decltype(m_buttons)>);
        static_assert(std::tuple_size_v<decltype(keys)> == std::tuple_size_v<decltype(m_buttons)>);
        for(int i = 0; i < buttonCount; i++) {
            FCoords from = m_gravity == ButtonGravity::left ? FCoords{0.f, buttonOffset + (float)i * buttonStride} : FCoords{buttonOffset + (float)i * buttonStride, 0.f};
            m_buttons[i] = {
                    m_instance.graphics().renderText(chars[i], "font/FFArrows.ttf", buttonSize, 0),
                    from,
                    from + FCoords{buttonSize, buttonSize},
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
    checkLongPress();
    drawButtons(target);
    drawDirpad(target);
}

void LevelInput::drawButtons(const DrawTarget& target) {
    auto& program = m_instance.graphics().shaders().button;
    glUseProgram(program);
    for(int i = 0; i < m_buttons.size(); i++) {
        glUniform2f(program.uniform("uPosition"), m_buttons[i].coordsFrom.fx(), m_buttons[i].coordsFrom.fy());
        auto& texture = m_buttons[i].texture;
        texture.bind();
        glUniform2f(program.uniform("uSrcSize"), (float) texture.width(), (float) texture.height());
        float alpha = !m_buttonsEnabled[i] ? .25f
                : m_dirpad.state == DirpadState::button && i == m_activeButton
                    ? 1.0f
                    : 0.5f;
        glUniform4fv(program.uniform("uColor"), 1, colorButtons.gl(alpha).data());
        GraphicsUtils::rect(0, 0, 1, 1);
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
    auto color = m_dirpad.fish == Model::Fish::small ? colorSmall : colorBig;

    for(auto dir : {Direction::up, Direction::down, Direction::left, Direction::right}) {
        glUniform2fv(program.uniform("uDirection"), 1, FCoords{dir}.gl().data());
        float alpha = (m_dirpad.state == DirpadState::follow && dir == m_dirpad.lastNonzeroDir ? 1.f : 0.5f) * baseAlpha;
        glUniform4fv(program.uniform("uColor"), 1, color.gl(alpha).data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

int LevelInput::findButton(FCoords pos) {
    for(int i = 0; i < m_buttons.size(); i++) {
        auto& button = m_buttons[i];
        if(pos.within(button.coordsFrom, button.coordsTo))
            return i;
    }
    return noButton;
}

void LevelInput::setButtonGravity(ButtonGravity gravity) {
    m_gravity = gravity;
}

FCoords LevelInput::getReserve() {
    switch(m_gravity) {
        case ButtonGravity::left:
            return {m_buttons[0].coordsTo.fx(), 0.f};
        case ButtonGravity::top:
            return {0.f, m_buttons[0].coordsTo.fy()};
    }
}
