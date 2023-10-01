#include "input.h"
#include "iosinstance.h"
#include "subsystem/graphics.h"
#include "bridge.h"

static constexpr float edgePercentage = 0.07f;
static constexpr float twoPointerMovePercentage = 0.1f;
static constexpr unsigned ringBufferSize = 16;

IOSInput::IOSInput(IOSInstance& instance) :
        m_instance(instance),
        m_pointerFollow(false),
        m_2pCenter(),
        m_head(0),
        m_tail(0)
{
    m_eventQueue.resize(ringBufferSize);
}

void IOSInput::reset() {
    std::lock_guard lock{m_mutex};
    m_pointerFollow = false;
}

bool IOSInput::registerTouchEvent(int action, FCoords coords) {
    std::lock_guard lock{m_mutex};
    switch(action) {
        case kTouchEventPointerDown: {
            auto size = m_instance.graphics().coords(Graphics::null).size;
            auto edge = edgePercentage * std::min(size.x, size.y);
            auto edges = FCoords{edge, edge};
            m_pointerFollow = true;
            enqueue(Events::PointerDown{coords});
            if(!coords.within(edges, size - edges)) {
                enqueue(Events::PointerUp{});
                m_pointerFollow = false;
                return false;
            }
            return true;
        }
        case kTouchEventPointerMove:
            if(!m_pointerFollow)
                return false;
            enqueue(Events::PointerMove{coords});
            return true;
        case kTouchEventPointerUp:
            if(!m_pointerFollow)
                return false;
            enqueue(Events::PointerUp{});
            m_pointerFollow = false;
            return true;
        case kTouchEventCancel:
            enqueue(Events::PointerCancel{});
            m_pointerFollow = false;
            return false;
        case kTouchEventTwoTap:
            m_2pCenter = coords;
            if(!m_pointerFollow)
                return false;
            enqueue(Events::TwoPointerTap{});
            return true;
        case kTouchEventTwoMove: {
            if(!m_2pCenter)
                return false;
            auto size = m_instance.graphics().coords(Graphics::null).size;
            auto minDist = twoPointerMovePercentage * std::min(size.x, size.y);
            auto dist = (coords - m_2pCenter.value()).length();
            if(dist > minDist) {
                Log::debug<Log::input>("sending UNDO");
                enqueue(Events::TwoPointerMove{});
                enqueue(Events::PointerCancel{});
                m_2pCenter.reset();
                m_pointerFollow = false;
            }
            return false;
        }
        default:
            return false;
    }
}

void IOSInput::enqueue(Event event) {
    auto tail = m_tail.load(std::memory_order::acquire);
    m_eventQueue[tail] = event;
    tail = (tail + 1) % ringBufferSize;
    m_tail.store(tail, std::memory_order::release);
}

static void deliver(IInputSink& sink, IOSInput::Events::PointerDown ev) {
    sink.pointerDown(ev.coords);
}

static void deliver(IInputSink& sink, IOSInput::Events::PointerMove ev) {
    sink.pointerMove(ev.coords);
}

static void deliver(IInputSink& sink, IOSInput::Events::PointerUp) {
    sink.pointerUp();
}

static void deliver(IInputSink& sink, IOSInput::Events::PointerCancel) {
    sink.pointerCancel();
}

static void deliver(IInputSink& sink, IOSInput::Events::TwoPointerTap) {
    sink.twoPointTap();
}

static void deliver(IInputSink& sink, IOSInput::Events::TwoPointerMove) {
    sink.keyDown(Key::undo);
}

void IOSInput::deliverEvents() {
    auto& inputSink = m_instance.inputSink();
    auto tail = m_tail.load(std::memory_order::acquire);
    while(m_head != tail) {
        auto& event = m_eventQueue[m_head];
        std::visit([&inputSink] (auto&& ev) { deliver(inputSink, ev); }, event);
        m_head = (m_head + 1) % ringBufferSize;
    }
}
