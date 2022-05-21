#include "instance.h"
#include "subsystem/input.h"
#include "game/screens/screenmanager.h"

bool BaseInput::handleKeyDown(Key key) {
    return m_instance.screens().dispatchKey(key);
}

bool BaseInput::handleKeyUp(Key) {
    return false;
}

bool BaseInput::handlePointerDown(FCoords pos) {
    auto windowCoords = m_instance.graphics().windowTarget().screen2window(pos);
    return m_instance.screens().dispatchMouse(windowCoords);
}

bool BaseInput::handlePointerMove(FCoords pos) {
    return false;
}

bool BaseInput::handlePointerUp() {
    return false;
}