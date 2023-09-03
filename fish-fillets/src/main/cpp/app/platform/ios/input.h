#ifndef FISH_FILLETS_IOS_INPUT_H
#define FISH_FILLETS_IOS_INPUT_H

#include "common.h"
#include "subsystem/input.h"
#include <variant>
#include <mutex>

class IOSInstance;

class IOSInput : public IInputSource {
public:
    IOSInput(IOSInstance& instance);

    bool registerTouchEvent(int action, FCoords coords);
    void deliverEvents();
    void reset() override;

    struct Events {
        struct PointerDown { FCoords coords; };
        struct PointerMove { FCoords coords; };
        struct PointerUp { };
        struct PointerCancel { };
        struct TwoPointer { };
    };

private:
    IOSInstance& m_instance;
    bool m_pointerFollow;

    using Event = std::variant<Events::PointerDown, Events::PointerMove, Events::PointerUp, Events::PointerCancel, Events::TwoPointer>;
    std::vector<Event> m_eventQueue;
    unsigned m_head;
    std::atomic<unsigned> m_tail;
    std::mutex m_mutex;

    void enqueue(Event event);
};

#endif //FISH_FILLETS_IOS_INPUT_H
