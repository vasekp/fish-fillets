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
    else if(m_dirpad.state == DirpadState::follow)
        return toKey(m_dirpad.lastDir);
    else
        return Key::none;
}

bool LevelInput::handlePointerDown(FCoords pos) {
    bool handled = false;
    if(auto button = findButton(pos); button != noButton && m_buttonsEnabled[button]) {
        m_dirpad.state = DirpadState::button;
        m_activeButton = button;
        return true;
    }
    if(std::chrono::steady_clock::now() < m_dirpad.touchTime + doubletapTime) {
        m_instance.screens().dispatchKey(Key::space);
        m_dirpad.touchTime = absolutePast;
        handled = true;
    } else
        m_dirpad.touchTime = std::chrono::steady_clock::now();
    if(m_dirpad.fish == Model::Fish::none) {
        m_dirpad.state = DirpadState::ignore;
        return handled;
    }
    auto windowCoords = m_instance.graphics().windowTarget().screen2window(pos);
    if(m_instance.screens().dispatchMouse(windowCoords)) {
        m_dirpad.state = DirpadState::ignore;
        return true;
    }
    m_dirpad.refPos = pos;
    m_dirpad.state = DirpadState::wait;
    return handled;
}

bool LevelInput::handlePointerMove(FCoords pos) {
    if(m_dirpad.state == DirpadState::button) {
        m_activeButton = findButton(pos);
        return true;
    }
    auto diff = pos - m_dirpad.refPos;
    bool small = diff.length() < minDistance * m_instance.graphics().dpi();
    ICoords dir{};
    if(diff.fx() > 2.f * std::abs(diff.fy()))
        dir = Direction::right;
    else if(diff.fx() < -2.f * std::abs(diff.fy()))
        dir = Direction::left;
    else if(diff.fy() > 2.f * std::abs(diff.fx()))
        dir = Direction::down;
    else if(diff.fy() < -2.f * std::abs(diff.fx()))
        dir = Direction::up;
    Log::verbose("move state=", (int)m_dirpad.state, " length=", diff.length(), " dir=", dir);
    switch (m_dirpad.state) {
        case DirpadState::idle:
            Log::error("Inconsistent dirpad state");
            return false;
        case DirpadState::ignore:
            return false;
        case DirpadState::wait:
            if(!small && dir) {
                m_instance.screens().dispatchKey(toKey(dir));
                m_dirpad.lastDir = dir;
                m_dirpad.touchTime = absolutePast;
                m_dirpad.state = DirpadState::follow;
                return true;
            } else
                return false;
        case DirpadState::follow:
            if(small || dir == m_dirpad.lastDir) {
                m_dirpad.refPos += (pos - m_dirpad.refPos).projectPositive(m_dirpad.lastDir);
            } else if(dir) {
                m_instance.screens().dispatchKey(toKey(dir));
                m_dirpad.lastDir = dir;
            } else {
                m_dirpad.state = DirpadState::wait;
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

Key LevelInput::toKey(ICoords dir) {
    if(dir == Direction::up)
        return Key::up;
    else if(dir == Direction::down)
        return Key::down;
    else if(dir == Direction::left)
        return Key::left;
    else if(dir == Direction::right)
        return Key::right;
    else
        return Key::none;
}

void LevelInput::refresh() {
    auto displayWidth = (float) m_instance.graphics().display().width();
    auto displayHeight = (float) m_instance.graphics().display().height();
    {
        auto& program = m_instance.graphics().shaders().arrow;
        glUseProgram(program);
        glUniform1f(program.uniform("uSize"), minDistance * m_instance.graphics().dpi());
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
        glUniform4fv(program.uniform("uColor"), 1, colorButtons.gl(alpha).get());
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

    using Vector = std::array<float, 2>;
    constexpr std::array<std::pair<ICoords, Vector>, 4> arrows{
            std::pair{Direction::up, Vector{0,  -1}},
            std::pair{Direction::down, Vector{0,  1}},
            std::pair{Direction::left, Vector{-1, 0}},
            std::pair{Direction::right, Vector{1,  0}}
    };

    constexpr float coords[3][3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };
    glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &coords[0][0]);

    glUniform2f(program.uniform("uPosition"), m_dirpad.refPos.fx(), m_dirpad.refPos.fy());
    auto color = m_dirpad.fish == Model::Fish::small ? colorSmall : colorBig;

    for(const auto& [dir, vector] : arrows) {
        glUniform2f(program.uniform("uDirection"), vector[0], vector[1]);
        float alpha = (m_dirpad.state == DirpadState::follow && dir == m_dirpad.lastDir ? 1.f : 0.5f) * baseAlpha;
        glUniform4fv(program.uniform("uColor"), 1, color.gl(alpha).get());
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
