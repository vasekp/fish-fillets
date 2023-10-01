#include "subsystem/input.h"
#include "xinput.h"
#include <X11/keysym.h>

static constexpr std::chrono::steady_clock::time_point absolutePast{};

XInput::XInput(Instance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_pointerFollow(false),
        m_lastHover(noHover)
{ }

void XInput::reset() {
    m_lastKey = Key::none;
    m_pointerFollow = false;
}

Key XKeyMap(KeySym keysym) {
    switch(keysym) {
        case XK_Up:
        case XK_KP_Up:
            return Key::up;
        case XK_Down:
        case XK_KP_Down:
            return Key::down;
        case XK_Left:
        case XK_KP_Left:
            return Key::left;
        case XK_Right:
        case XK_KP_Right:
            return Key::right;
        case XK_w:
            return Key::bigUp;
        case XK_s:
            return Key::bigDown;
        case XK_a:
            return Key::bigLeft;
        case XK_d:
            return Key::bigRight;
        case XK_i:
            return Key::smallUp;
        case XK_k:
            return Key::smallDown;
        case XK_j:
            return Key::smallLeft;
        case XK_l:
            return Key::smallRight;
        case XK_space:
            return Key::space;
        case XK_Escape:
        case XK_q:
            return Key::exit;
        case XK_F2:
            return Key::save;
        case XK_F3:
            return Key::load;
        case XK_BackSpace:
            return Key::restart;
        case XK_u:
            return Key::undo;
        default:
            return Key::none;
    }
}

void XInput::keyEvent(const XKeyEvent& event) {
    if(event.type == KeyPress) {
        if(m_lastKey == Key::none) {
            auto key = XKeyMap(XLookupKeysym(const_cast<XKeyEvent*>(&event), 0));
            if(key != Key::none) {
              m_instance.inputSink().keyDown(key);
              m_lastKey = key;
            }
        }
        return;
    } else {
        m_lastKey = Key::none;
    }
}

void XInput::buttonEvent(const XButtonEvent& event) {
    FCoords coords{event.x, event.y};
    auto& inputSink = m_instance.inputSink();
    switch(event.type) {
    case ButtonPress:
        if(event.button == Button1) {
            m_pointerFollow = true;
            inputSink.pointerDown(coords);
        } else if(event.button == Button3) {
            inputSink.twoPointTap();
        }
        break;
    case ButtonRelease:
        if(event.button == Button1) {
            if(!m_pointerFollow)
                return;
            inputSink.pointerUp();
            m_pointerFollow = false;
        }
        break;
    }
}

void XInput::motionEvent(const XMotionEvent& event) {
    FCoords coords{event.x, event.y};
    m_lastHover = coords;
    if(!m_pointerFollow || !(event.state & Button1Mask))
        return;
    m_instance.inputSink().pointerMove(coords);
}

Key XInput::pollKey() {
    return m_lastKey;
}

FCoords XInput::hover() {
    return m_lastHover;
}
