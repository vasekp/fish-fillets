#include "subsystem/input.h"
#include "instance.h"
#include "game/screens/screenmanager.h"
#include "./input.h"
#include <X11/keysym.h>

XInput::XInput(Instance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_keyHandled(false),
        m_pointerFollow(false),
        m_lastPointerDownTime(absolutePast),
        m_pointerDownTime(absolutePast),
        m_pointerHandled(false)
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
            return Key::exit; // TODO rename to quit
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
    auto& input = m_instance.screens().curScreen().input();
    if(event.type == KeyPress) {
        if(m_lastKey == Key::none) {
            m_lastKey = XKeyMap(XLookupKeysym(&event, 0));
            input.keyDown(m_lastKey);
        }
        return;
    } else {
        m_lastKey = Key::none;
        // TODO remove input.keyUp
    }
}

void XInput::buttonEvent(const XButtonEvent& event) {
    auto& input = m_instance.screens().curScreen().input();
    if(event.type == ButtonPress) {
        FCoords coords{event.x, event.y};
        if(event.button == Button1) {
            m_pointerDownTime = std::chrono::steady_clock::now();
            m_pointerDownCoords = coords;
            m_pointerFollow = true;
            if(m_pointerDownTime - m_lastPointerDownTime < doubletapTime)
                m_pointerHandled = input.doubleTap(coords);
            else
                m_pointerHandled = input.pointerDown(coords);
        } // TODO right click
    } else {
        if(event.button == Button1) {
            if(!m_pointerFollow)
                return;
            m_pointerHandled |= input.pointerUp(!m_pointerHandled);
            m_lastPointerDownTime = m_pointerDownTime;
            m_pointerDownTime = absolutePast;
            m_pointerFollow = false;
        }
    }
}
#if 0
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        } else if(action == AMOTION_EVENT_ACTION_MOVE) {
            if(!m_pointerFollow)
                return false;
            else
                return m_pointerHandled |= input.pointerMove(coords);
        }
    }
#endif

void XInput::motionEvent(const XMotionEvent& event) { }

void XInput::ping() {
    if(m_pointerDownTime != absolutePast && std::chrono::steady_clock::now() > m_pointerDownTime + longpressTime) {
        m_pointerHandled |= m_instance.screens().curScreen().input().longPress(m_pointerDownCoords);
        m_pointerDownTime = absolutePast;
    }
}

Key XInput::poolKey() {
    return m_lastKey;
}
