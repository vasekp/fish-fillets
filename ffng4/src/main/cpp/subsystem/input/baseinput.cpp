#include "instance.h"
#include "subsystem/input.h"
#include "game/screens/screenmanager.h"

bool BaseInput::keyDown(Key key) {
    return m_instance.screens().dispatchKey(key);
}

bool BaseInput::pointerDown(FCoords coords) {
    auto windowCoords = m_instance.graphics().windowTarget().screen2window(coords);
    return m_instance.screens().dispatchPointer(windowCoords);
}