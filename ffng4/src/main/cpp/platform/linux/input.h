#ifndef FISH_FILLETS_ANDROID_INPUT_H
#define FISH_FILLETS_ANDROID_INPUT_H

#include "subsystem/input.h"
#include "xlib-fenced.h"

class XInstance;

class XInput : public IInputSource {
    Instance& m_instance;

    Key m_lastKey;
    bool m_keyHandled;

    bool m_pointerFollow;
    std::chrono::steady_clock::time_point m_lastPointerDownTime;
    std::chrono::steady_clock::time_point m_pointerDownTime;
    FCoords m_pointerDownCoords;
    bool m_pointerHandled;

    constexpr static std::chrono::steady_clock::duration doubletapTime = std::chrono::milliseconds(300);
    constexpr static std::chrono::steady_clock::duration longpressTime = std::chrono::milliseconds(500);
    constexpr static std::chrono::steady_clock::time_point absolutePast{};

public:
    XInput(Instance& instance);

    void keyEvent(XKeyEvent& xkey);
    void buttonEvent(const XButtonEvent& xbutton);
    void motionEvent(const XMotionEvent& xmotion);
    void ping();
    Key poolKey() override;
    void reset() override;
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
