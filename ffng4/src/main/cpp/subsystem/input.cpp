#include "input.h"
#include "instance.h"
#include "game/screens/screenmanager.h"

Input::Input(Instance& instance) :
    m_instance(instance),
    m_lastKey(Key::none),
    m_dirpad({DirpadState::idle}),
    m_density(72.f)
{ }

void Input::setDensity(float density) {
    m_density = density;
}

unsigned Input::index(Key key) {
    return (unsigned)key;
}

bool Input::handleKeyDown(Key key) {
    m_lastKey = key;
    return m_instance.screens().dispatchKey(key);
}

bool Input::handleKeyUp(Key) {
    m_lastKey = Key::none;
    return false;
}

Key Input::pool() {
    if(m_lastKey != Key::none)
        return m_lastKey;
    else if(m_dirpad.state == DirpadState::follow)
        return toKey(m_dirpad.lastDir);
    else
        return Key::none;
}

bool Input::handlePointerDown(FCoords pos) {
    auto windowCoords = m_instance.graphics().windowTarget().screen2window(pos);
    if(m_instance.screens().dispatchMouse(windowCoords)) {
        m_dirpad.state = DirpadState::ignore;
        return true;
    }
    if(std::chrono::steady_clock::now() < m_dirpad.touchTime + doubletapTime) {
        m_instance.screens().dispatchKey(Key::space);
        m_dirpad.touchTime = off;
    } else
        m_dirpad.touchTime = std::chrono::steady_clock::now();
    m_dirpad.refPos = pos;
    m_dirpad.state = DirpadState::wait;
    return true;
}

bool Input::handlePointerMove(FCoords pos) {
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
                m_dirpad.touchTime = off;
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

bool Input::handlePointerUp() {
    m_dirpad.state = DirpadState::idle;
    return false;
}

Key Input::toKey(ICoords dir) {
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