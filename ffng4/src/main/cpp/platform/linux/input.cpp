#include "subsystem/input.h"
#include "instance.h"
#include "./input.h"
#include <X11/keysym.h>

XInput::XInput(Instance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_keyHandled(false),
        m_pointerFollow(false),
        m_lastPointerDownTime(absolutePast),
        m_pointerDownTime(absolutePast),
        m_pointerHandled(false),
        m_lastHover(noHover)
{ }

void XInput::reset() {
    m_lastKey = Key::none;
    m_pointerFollow = false;
    m_lastPointerDownTime = absolutePast;
    m_pointerDownTime = absolutePast;
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
        default:
            return Key::none;
    }
}

void XInput::keyEvent(XKeyEvent& event) {
    if(event.type == KeyPress) {
        if(m_lastKey == Key::none) {
            m_lastKey = XKeyMap(XLookupKeysym(&event, 0));
            m_instance.inputSink().keyDown(m_lastKey);
        }
        return;
    } else {
        m_lastKey = Key::none;
        // TODO remove input.keyUp
    }
}

void XInput::buttonEvent(const XButtonEvent& event) {
    FCoords coords{event.x, event.y};
    auto& inputSink = m_instance.inputSink();
    switch(event.type) {
    case ButtonPress:
        if(event.button == Button1) {
            m_pointerDownTime = std::chrono::steady_clock::now();
            m_pointerDownCoords = coords;
            m_pointerFollow = true;
            if(m_pointerDownTime - m_lastPointerDownTime < doubletapTime)
                m_pointerHandled = inputSink.doubleTap(coords);
            else
                m_pointerHandled = inputSink.pointerDown(coords);
        } else if(event.button == Button3) {
            m_pointerHandled |= inputSink.twoPointTap();
        }
        break;
    case ButtonRelease:
        if(event.button == Button1) {
            if(!m_pointerFollow)
                return;
            m_pointerHandled |= inputSink.pointerUp(!m_pointerHandled);
            m_lastPointerDownTime = m_pointerDownTime;
            m_pointerDownTime = absolutePast;
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
    m_pointerHandled |= m_instance.inputSink().pointerMove(coords);
}

void XInput::ping() {
    if(m_pointerDownTime != absolutePast && std::chrono::steady_clock::now() > m_pointerDownTime + longpressTime) {
        m_pointerHandled |= m_instance.inputSink().longPress(m_pointerDownCoords);
        m_pointerDownTime = absolutePast;
    }
}

Key XInput::poolKey() {
    return m_lastKey;
}

FCoords XInput::hover() {
    return m_lastHover;
}
