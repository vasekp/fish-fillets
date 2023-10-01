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
    Key pollKey() override;
    void reset() override;
    FCoords hover() override;

private:
    AndroidInstance& m_instance;

    Key m_lastKey;

    bool m_pointerFollow;
    std::int32_t m_pointerId;
    FCoords m_lastHover;

    std::optional<FCoords> m_2pCenter;
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
