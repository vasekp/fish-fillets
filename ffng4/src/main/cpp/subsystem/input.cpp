#include "input.h"
#include "instance.h"
#include "game/screens/screenmanager.h"

unsigned Input::index(Key key) {
    return (unsigned)key;
}

bool Input::handleKeyDown(Key key) {
    LOGV("key down %d", index(key));
    m_pressed.set(index(key));
    return m_instance.screens().dispatchKey(key);
}

bool Input::handleKeyUp(Key key) {
    LOGV("key up %d", index(key));
    m_pressed.reset(index(key));
    return true;
}

bool Input::pressed(Key key) {
    return m_pressed[index(key)];
}