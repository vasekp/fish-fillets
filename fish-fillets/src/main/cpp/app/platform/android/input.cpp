#include "input.h"
#include "ainstance.h"
#include "subsystem/graphics.h"

static constexpr std::chrono::steady_clock::time_point absolutePast{};
static constexpr float edgePercentage = 0.07f;
static constexpr float twoPointerMovePercentage = 0.1f;

AndroidInput::AndroidInput(AndroidInstance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_pointerFollow(false),
        m_pointerId(-1),
        m_lastHover(noHover),
        m_2pCenter()
{ }

void AndroidInput::reset() {
    m_lastKey = Key::none;
    m_pointerFollow = false;
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
    auto& inputSink = m_instance.inputSink();
    auto& jni = m_instance.jni;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        auto combined = AMotionEvent_getAction(event);
        auto action = combined & AMOTION_EVENT_ACTION_MASK;
        auto index = (combined & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        if(action == AMOTION_EVENT_ACTION_CANCEL) {
            inputSink.pointerCancel();
            m_pointerFollow = false;
            return false;
        }
        auto pointerCount = AMotionEvent_getPointerCount(event);
        Log::verbose<Log::input>("action: ", action, " pointers: ", pointerCount, " primary: ", AMotionEvent_getPointerId(event, 0));
        if(pointerCount == 1) {
            auto pointerId = AMotionEvent_getPointerId(event, 0);
            FCoords coords{AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
            switch(action) {
                case AMOTION_EVENT_ACTION_DOWN: {
                    auto size = m_instance.graphics().coords(Graphics::null).size;
                    auto edge = edgePercentage * std::min(size.x, size.y);
                    auto edges = FCoords{edge, edge};
                    m_pointerId = pointerId;
                    m_pointerFollow = true;
                    auto ret = inputSink.pointerDown(coords);
                    if(!coords.within(edges, size - edges) && !ret) {
                        inputSink.pointerUp();
                        m_pointerFollow = false;
                        return false;
                    }
                    return true;
                }
                case AMOTION_EVENT_ACTION_MOVE: {
                    if(!m_pointerFollow || pointerId != m_pointerId)
                        return false;
                    inputSink.pointerMove(coords);
                    return true;
                }
                case AMOTION_EVENT_ACTION_UP:
                    if(!m_pointerFollow)
                        return false;
                    if(pointerId == m_pointerId)
                        inputSink.pointerUp();
                    m_pointerFollow = false;
                    return true;
                case AMOTION_EVENT_ACTION_HOVER_ENTER:
                    [[fallthrough]];
                case AMOTION_EVENT_ACTION_HOVER_MOVE:
                    m_lastHover = coords;
                    return false;
                case AMOTION_EVENT_ACTION_HOVER_EXIT:
                    m_lastHover = noHover;
                    return false;
                default:
                    return false;
            }
        } else if(pointerCount == 2) {
            auto id1 = AMotionEvent_getPointerId(event, 0);
            auto id2 = AMotionEvent_getPointerId(event, 1);
            FCoords coords1{AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
            FCoords coords2{AMotionEvent_getX(event, 1), AMotionEvent_getY(event, 1)};
            FCoords center = (coords1 + coords2) / 2.f;
            switch(action) {
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    Log::debug<Log::input>("secondary: ", AMotionEvent_getPointerId(event, 1));
                    if(m_pointerFollow && (id1 == m_pointerId || id2 == m_pointerId))
                        inputSink.twoPointTap();
                    m_2pCenter = center;
                    return true;
                case AMOTION_EVENT_ACTION_MOVE: {
                    if(!m_2pCenter)
                        return false;
                    auto size = m_instance.graphics().coords(Graphics::null).size;
                    auto minDist = twoPointerMovePercentage * std::min(size.x, size.y);
                    auto dist = (center - m_2pCenter.value()).length();
                    if(dist > minDist) {
                        Log::debug<Log::input>("sending UNDO");
                        inputSink.keyDown(Key::undo);
                        m_2pCenter.reset();
                        inputSink.pointerCancel();
                        m_pointerFollow = false;
                    }
                    return false;
                }
                case AMOTION_EVENT_ACTION_POINTER_UP:
                    m_2pCenter.reset();
                    if(AMotionEvent_getPointerId(event, index) == m_pointerId) {
                        inputSink.pointerUp();
                        m_pointerFollow = false;
                        return true;
                    }
                    return false;
                default:
                    return false;
            }
        } else
            m_2pCenter.reset();
    } else if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        auto key = AndroidKeymap(AKeyEvent_getKeyCode(event));
        if(key == Key::none)
            return false;
        auto action = AKeyEvent_getAction(event);
        if(action == AKEY_EVENT_ACTION_DOWN) {
            if(m_lastKey != Key::none)
                return false;
            m_lastKey = key;
            return inputSink.keyDown(key);
        } else if(action == AKEY_EVENT_ACTION_UP) {
            if(key != m_lastKey)
                return false;
            m_lastKey = Key::none;
            return true;
        } else
            return false;
    }
    return false;
}

Key AndroidInput::pollKey() {
    return m_lastKey;
}

FCoords AndroidInput::hover() {
    return m_lastHover;
}
