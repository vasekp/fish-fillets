#include "input.h"
#include "ainstance.h"
#include "subsystem/graphics.h"

static constexpr float edgePercentage = 0.07f;
static constexpr unsigned ringBufferSize = 16;

AndroidInput::AndroidInput(AndroidInstance& instance) :
        m_instance(instance),
        m_lastKey(Key::none),
        m_pointerFollow(false),
        m_lastHover(noHover),
        m_head(0),
        m_tail(0)
{
    m_eventQueue.resize(ringBufferSize);
}

void AndroidInput::reset() {
    std::lock_guard lock{m_mutex};
    m_lastKey.store(Key::none, std::memory_order::relaxed);
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

bool AndroidInput::registerTouchEvent(int action, FCoords coords) {
    std::lock_guard lock{m_mutex};
    switch(action) {
        case AMOTION_EVENT_ACTION_DOWN: {
            auto size = m_instance.graphics().coords(Graphics::null).size; // TODO: no graphics(), only if fullscreen
            auto edge = edgePercentage * std::min(size.x, size.y);
            auto edges = FCoords{edge, edge};
            m_pointerFollow = true;
            enqueue(Events::PointerDown{coords});
            if(!coords.within(edges, size - edges) /*&& !ret*/) { // TODO
                enqueue(Events::PointerUp{});
                m_pointerFollow = false;
                return false;
            }
            return true;
        }
        case AMOTION_EVENT_ACTION_MOVE:
            if(!m_pointerFollow)
                return false;
            enqueue(Events::PointerMove{coords});
            return true;
        case AMOTION_EVENT_ACTION_UP:
            if(!m_pointerFollow)
                return false;
            enqueue(Events::PointerUp{});
            m_pointerFollow = false;
            return true;
        case AMOTION_EVENT_ACTION_CANCEL:
            enqueue(Events::PointerCancel{});
            m_pointerFollow = false;
            return false;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            if(!m_pointerFollow)
                return false;
            enqueue(Events::TwoPointer{});
            return true;
        case AMOTION_EVENT_ACTION_HOVER_ENTER:
            [[fallthrough]];
        case AMOTION_EVENT_ACTION_HOVER_MOVE:
            m_lastHover.store(coords, std::memory_order::relaxed);
            return false;
        case AMOTION_EVENT_ACTION_HOVER_EXIT:
            m_lastHover.store(noHover, std::memory_order::relaxed);
            return false;
        default:
            return false;
    }
}

bool AndroidInput::registerKeyEvent(int action, int keyCode) {
    std::lock_guard lock{m_mutex};
    auto key = AndroidKeymap(keyCode);
    if(key == Key::none)
        return false;
    auto last = m_lastKey.load(std::memory_order::relaxed);
    if(action == AKEY_EVENT_ACTION_DOWN) {
        if(last != Key::none)
            return false;
        m_lastKey.store(key, std::memory_order::relaxed);
        enqueue(Events::KeyDown{key});
        return true;
    } else if(action == AKEY_EVENT_ACTION_UP) {
        if(key != last)
            return false;
        m_lastKey.store(Key::none, std::memory_order::relaxed);
        return true;
    } else
        return false;
}

void AndroidInput::enqueue(Event event) {
    auto tail = m_tail.load(std::memory_order::acquire);
    m_eventQueue[tail] = event;
    tail = (tail + 1) % ringBufferSize;
    m_tail.store(tail, std::memory_order::release);
}

static void deliver(IInputSink& sink, AndroidInput::Events::KeyDown ev) {
    sink.keyDown(ev.key);
}

static void deliver(IInputSink& sink, AndroidInput::Events::PointerDown ev) {
    sink.pointerDown(ev.coords);
}

static void deliver(IInputSink& sink, AndroidInput::Events::PointerMove ev) {
    sink.pointerMove(ev.coords);
}

static void deliver(IInputSink& sink, AndroidInput::Events::PointerUp) {
    sink.pointerUp();
}

static void deliver(IInputSink& sink, AndroidInput::Events::PointerCancel) {
    sink.pointerCancel();
}

static void deliver(IInputSink& sink, AndroidInput::Events::TwoPointer) {
    sink.twoPointTap();
}

void AndroidInput::deliverEvents() {
    auto& inputSink = m_instance.inputSink();
    auto tail = m_tail.load(std::memory_order::acquire);
    while(m_head != tail) {
        auto& event = m_eventQueue[m_head];
        std::visit([&inputSink] (auto&& ev) { deliver(inputSink, ev); }, event);
        m_head = (m_head + 1) % ringBufferSize;
    }
}

Key AndroidInput::pollKey() {
    return m_lastKey.load(std::memory_order::relaxed);
}

FCoords AndroidInput::hover() {
    return m_lastHover.load(std::memory_order::relaxed);
}
