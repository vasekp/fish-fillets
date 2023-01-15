#include "baseinput.h"
#include "instance.h"
#include "game/screens/screen.h"

bool BaseInput::keyDown(Key key) {
    return m_screen.keypress(key);
}

bool BaseInput::pointerDown(FCoords coords) {
    auto windowCoords = m_instance.graphics().coords(Graphics::CoordSystems::window).out2in(coords);
    return m_screen.pointer(windowCoords);
}