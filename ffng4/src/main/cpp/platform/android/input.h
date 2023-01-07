#ifndef FISH_FILLETS_ANDROID_INPUT_H
#define FISH_FILLETS_ANDROID_INPUT_H

#include "subsystem/input.h"
#include <android/input.h>

class AndroidInput : public IInputSource {
    Instance& m_instance;

    Key m_lastKey;
    bool m_keyHandled;

    bool m_pointerFollow;
    std::int32_t m_pointerId;
    std::chrono::steady_clock::time_point m_pointerDownTime;
    FCoords m_pointerDownCoords;
    bool m_pointerHandled;

    constexpr static std::chrono::steady_clock::duration doubletapTime = std::chrono::milliseconds(300);
    constexpr static std::chrono::steady_clock::duration longpressTime = std::chrono::milliseconds(500);
    constexpr static std::chrono::steady_clock::time_point absolutePast{};

public:
    AndroidInput(Instance& instance);

    bool processEvent(AInputEvent* event);
    void ping();
    Key poolKey() override;
    void reset() override;
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
