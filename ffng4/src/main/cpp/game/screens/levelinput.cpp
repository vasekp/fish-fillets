#include "levelinput.h"
#include "instance.h"
#include "game/screens/screenmanager.h"
#include "subsystem/graphics.h"

LevelInput::LevelInput(Instance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_dirpad({DirpadState::ignore}),
        m_density(72.f)
{ }

void LevelInput::setDensity(float density) {
    m_density = density;
}

void LevelInput::setFish(Model::Fish fish) {
    m_dirpad.fish = fish;
    if(fish == Model::Fish::none)
        m_dirpad.state = DirpadState::ignore;;
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
    if(m_dirpad.fish == Model::Fish::none)
        return false;
    auto windowCoords = m_instance.graphics().windowTarget().screen2window(pos);
    if(m_instance.screens().dispatchMouse(windowCoords)) {
        m_dirpad.state = DirpadState::ignore;
        return true;
    }
    if(std::chrono::steady_clock::now() < m_dirpad.touchTime + doubletapTime) {
        m_instance.screens().dispatchKey(Key::space);
        m_dirpad.touchTime = absolutePast;
    } else
        m_dirpad.touchTime = std::chrono::steady_clock::now();
    m_dirpad.refPos = pos;
    m_dirpad.state = DirpadState::wait;
    return true;
}

bool LevelInput::handlePointerMove(FCoords pos) {
    auto diff = pos - m_dirpad.refPos;
    bool small = diff.length() < m_density * minDistance;
    ICoords dir{};
    if(diff.fx() > 2.f * std::abs(diff.fy()))
        dir = Direction::right;
    else if(diff.fx() < -2.f * std::abs(diff.fy()))
        dir = Direction::left;
    else if(diff.fy() > 2.f * std::abs(diff.fx()))
        dir = Direction::down;
    else if(diff.fy() < -2.f * std::abs(diff.fx()))
        dir = Direction::up;
    LOGV("move state=%d length=%f dir=[%d,%d]", (int)m_dirpad.state, diff.length(), dir.x, dir.y);
    switch (m_dirpad.state) {
        case DirpadState::idle:
            ::error("Inconsistent dirpad state");
        case DirpadState::ignore:
            break;
        case DirpadState::wait:
            if(!small && dir) {
                m_instance.screens().dispatchKey(toKey(dir));
                m_dirpad.lastDir = dir;
                m_dirpad.touchTime = absolutePast;
                m_dirpad.state = DirpadState::follow;
            }
            return false;
        case DirpadState::follow:
            if(small || dir == m_dirpad.lastDir) {
                m_dirpad.refPos += (pos - m_dirpad.refPos).projectPositive(m_dirpad.lastDir);
                return false;
            } else if(dir) {
                m_instance.screens().dispatchKey(toKey(dir));
                m_dirpad.lastDir = dir;
                return true;
            } else {
                m_dirpad.state = DirpadState::wait;
                return false;
            }
    }
    return false;
}

bool LevelInput::handlePointerUp() {
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
    auto& program = m_instance.graphics().shaders().arrow;
    glUseProgram(program);
    glUniform1f(program.uniform("uSize"), m_density * minDistance);
    glUniform2f(program.uniform("uDstSize"), (float)m_instance.graphics().display().width(), (float)m_instance.graphics().display().height());
}

void LevelInput::draw(const DrawTarget& target) {
    if(m_dirpad.state == DirpadState::idle || m_dirpad.state == DirpadState::ignore)
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

    using Matrix = std::array<float, 4>;
    constexpr std::array<std::pair<ICoords, Matrix>, 4> matrices{
            std::pair{Direction::up, Matrix{1, 0, 0,  -1}},
            std::pair{Direction::down, Matrix{1, 0, 0,  1}},
            std::pair{Direction::left, Matrix{0, 1, -1, 0}},
            std::pair{Direction::right, Matrix{0, 1, 1,  0}}
    };

    float coords[3][3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };
    glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &coords[0][0]);

    glUniform2f(program.uniform("uPosition"), m_dirpad.refPos.fx(), m_dirpad.refPos.fy());
    auto color = m_dirpad.fish == Model::Fish::small ? colorSmall : colorBig;

    for(const auto& [dir, matrix] : matrices) {
        glUniformMatrix2fv(program.uniform("uMatrix"), 1, false, &matrix[0]);
        float alpha = (m_dirpad.state == DirpadState::follow && dir == m_dirpad.lastDir ? 1.f : 0.5f) * baseAlpha;
        glUniform4fv(program.uniform("uColor"), 1, color.gl(alpha).get());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}