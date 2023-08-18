#ifndef FISH_FILLETS_ANDROID_INPUT_H
#define FISH_FILLETS_ANDROID_INPUT_H

#include "common.h"
#include "subsystem/input.h"
#include <android/input.h>

class AndroidInstance;

class AndroidInput : public IInputSource {
public:
    AndroidInput(AndroidInstance& instance);

    bool processEvent(AInputEvent* event);
    void ping() override;
    Key pollKey() override;
    void reset() override;
    FCoords hover() override;

private:
    AndroidInstance& m_instance;

    Key m_lastKey;
    bool m_keyHandled;

    bool m_pointerFollow;
    std::int32_t m_pointerId;
    std::chrono::steady_clock::time_point m_pointerDownTime;
    FCoords m_pointerCoords;
    bool m_pointerHandled;
    FCoords m_lastHover;
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
