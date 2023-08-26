#ifndef FISH_FILLETS_ANDROID_INPUT_H
#define FISH_FILLETS_ANDROID_INPUT_H

#include "common.h"
#include "subsystem/input.h"
#include <android/input.h>
#include <variant>
#include <mutex>

class AndroidInstance;

class AndroidInput : public IInputSource {
public:
    AndroidInput(AndroidInstance& instance);

    bool registerTouchEvent(int action, FCoords coords);
    bool registerKeyEvent(int action, int key);

    void deliverEvents();
    Key pollKey() override;
    void reset() override;
    FCoords hover() override;

    struct Events {
        struct KeyDown { Key key; };
        struct PointerDown { FCoords coords; };
        struct PointerMove { FCoords coords; };
        struct PointerUp { };
        struct PointerCancel { };
        struct TwoPointer { };
    };

private:
    AndroidInstance& m_instance;
    std::atomic<Key> m_lastKey;
    bool m_pointerFollow;
    std::atomic<FCoords> m_lastHover;

    using Event = std::variant<Events::KeyDown, Events::PointerDown, Events::PointerMove, Events::PointerUp, Events::PointerCancel, Events::TwoPointer>;
    std::vector<Event> m_eventQueue;
    unsigned m_head;
    std::atomic<unsigned> m_tail;
    std::mutex m_mutex;

    void enqueue(Event event);
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
