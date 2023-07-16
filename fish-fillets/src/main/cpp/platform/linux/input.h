#ifndef FISH_FILLETS_ANDROID_INPUT_H
#define FISH_FILLETS_ANDROID_INPUT_H

#include "subsystem/input.h"
#include <X11/Xlib.h>

class XInstance;

class XInput : public IInputSource {
    Instance& m_instance;

    Key m_lastKey;
    bool m_keyHandled;

    bool m_pointerFollow;
    std::chrono::steady_clock::time_point m_lastPointerDownTime;
    std::chrono::steady_clock::time_point m_pointerDownTime;
    FCoords m_pointerCoords;
    bool m_pointerHandled;
    FCoords m_lastHover;

    constexpr static std::chrono::steady_clock::duration doubletapTime = 300ms;
    constexpr static std::chrono::steady_clock::duration longpressTime = 500ms;
    constexpr static std::chrono::steady_clock::time_point absolutePast{};

public:
    XInput(Instance& instance);

    void keyEvent(XKeyEvent& xkey);
    void buttonEvent(const XButtonEvent& xbutton);
    void motionEvent(const XMotionEvent& xmotion);
    void ping() override;
    Key poolKey() override;
    void reset() override;
    FCoords hover() override;
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
