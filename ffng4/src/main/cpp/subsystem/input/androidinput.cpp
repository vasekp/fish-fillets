#include "subsystem/input.h"
#include "game/screens/screenmanager.h"
#include "game/screens/screen.h"
#include "instance.h"

AndroidInput::AndroidInput(Instance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_keyHandled(false),
        m_pointerFollow(false),
        m_pointerId(-1),
        m_lastPointerDownTime(absolutePast),
        m_pointerDownTime(absolutePast),
        m_pointerHandled(false)
{ }

void AndroidInput::reset() {
    m_lastKey = Key::none;
    m_pointerFollow = false;
    m_lastPointerDownTime = absolutePast;
    m_pointerDownTime = absolutePast;
}

static Key AndroidKeymap(unsigned int code) {
    switch (code) {
        case AKEYCODE_DPAD_UP:
            return Key::up;
        case AKEYCODE_DPAD_DOWN:
            return Key::down;
        case AKEYCODE_DPAD_LEFT:
            return Key::left;
        case AKEYCODE_DPAD_RIGHT:
            return Key::right;
        case AKEYCODE_SPACE:
        case AKEYCODE_DPAD_CENTER:
            return Key::space;
        case AKEYCODE_ESCAPE:
        case AKEYCODE_BACK:
        case AKEYCODE_Q:
            return Key::exit;
        case AKEYCODE_F2:
        case AKEYCODE_S:
            return Key::save;
        case AKEYCODE_F3:
        case AKEYCODE_L:
            return Key::load;
        case AKEYCODE_DEL:
        case AKEYCODE_R:
            return Key::restart;
        default:
            return Key::none;
    }
}

bool AndroidInput::processEvent(AInputEvent* event) {
    auto& input = m_instance.screens().curScreen().input();
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        auto action = AMotionEvent_getAction(event);
        if(action == AMOTION_EVENT_ACTION_CANCEL) {
            m_pointerDownTime = absolutePast;
            m_lastPointerDownTime = absolutePast;
            m_pointerFollow = false;
            return false;
        }
        auto pointerCount = AMotionEvent_getPointerCount(event);
        Log::debug("action: ", action, " pointers: ", pointerCount, " primary: ", AMotionEvent_getPointerId(event, 0));
        if(pointerCount == 1) {
            auto pointerId = AMotionEvent_getPointerId(event, 0);
            FCoords coords{AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
            if(action == AMOTION_EVENT_ACTION_DOWN) {
                m_pointerDownTime = std::chrono::steady_clock::now();
                m_pointerDownCoords = coords;
                m_pointerId = pointerId;
                m_pointerFollow = true;
                if(m_pointerDownTime - m_lastPointerDownTime < doubletapTime)
                    m_pointerHandled = input.doubleTap(coords);
                else
                    m_pointerHandled = input.pointerDown(coords);
                m_instance.jni()->CallVoidMethod(m_instance.jni().object(), m_instance.jni().method("hideUI"));
            } else if(action == AMOTION_EVENT_ACTION_MOVE) {
                if(!m_pointerFollow || pointerId != m_pointerId)
                    return false;
                else
                    return m_pointerHandled |= input.pointerMove(coords);
            } else if(action == AMOTION_EVENT_ACTION_UP) {
                if(!m_pointerFollow)
                    return false;
                if(pointerId == m_pointerId)
                    m_pointerHandled |= input.pointerUp(!m_pointerHandled);
                if(!m_pointerHandled)
                    m_instance.jni()->CallVoidMethod(m_instance.jni().object(), m_instance.jni().method("showUI"));
                else
                    m_instance.jni()->CallVoidMethod(m_instance.jni().object(), m_instance.jni().method("hideUI"));
                m_lastPointerDownTime = m_pointerDownTime;
                m_pointerDownTime = absolutePast;
                m_pointerFollow = false;
                return m_pointerHandled;
            }
        } else if(pointerCount == 2 && (action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            Log::debug("secondary: ", AMotionEvent_getPointerId(event, 1));
            if(!m_pointerFollow)
                return false;
            auto id0 = AMotionEvent_getPointerId(event, 0);
            auto id1 = AMotionEvent_getPointerId(event, 1);
            if(id0 != m_pointerId && id1 != m_pointerId)
                return false;
            return m_pointerHandled |= input.twoPointTap();
        }
    } else if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        auto key = AndroidKeymap(AKeyEvent_getKeyCode(event));
        auto action = AKeyEvent_getAction(event);
        if(action == AKEY_EVENT_ACTION_DOWN) {
            if(m_lastKey != Key::none)
                return false;
            m_lastKey = key;
            return m_keyHandled = input.keyDown(key);
        } else if(action == AKEY_EVENT_ACTION_UP) {
            if(key != m_lastKey)
                return false;
            m_lastKey = Key::none;
            return m_keyHandled | input.keyUp(key);
        } else
            return false;
    }
    return false;
}

void AndroidInput::ping() {
    if(m_pointerDownTime != absolutePast && std::chrono::steady_clock::now() > m_pointerDownTime + longpressTime) {
        m_pointerHandled |= m_instance.screens().curScreen().input().longPress(m_pointerDownCoords);
        m_pointerDownTime = absolutePast;
    }
}

Key AndroidInput::poolKey() {
    return m_lastKey;
}