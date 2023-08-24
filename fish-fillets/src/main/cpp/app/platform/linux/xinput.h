#ifndef FISH_FILLETS_ANDROID_INPUT_H
#define FISH_FILLETS_ANDROID_INPUT_H

#include "common.h"
#include "subsystem/input.h"
#include <X11/Xlib.h>

class XInput : public IInputSource {
public:
    XInput(Instance& instance);

    void keyEvent(const XKeyEvent& xkey);
    void buttonEvent(const XButtonEvent& xbutton);
    void motionEvent(const XMotionEvent& xmotion);
    Key pollKey() override;
    void reset() override;
    FCoords hover() override;

private:
    Instance& m_instance;

    Key m_lastKey;
    bool m_keyHandled;

    bool m_pointerFollow;
    std::chrono::steady_clock::time_point m_lastPointerDownTime;
    std::chrono::steady_clock::time_point m_pointerDownTime;
    FCoords m_pointerCoords;
    bool m_pointerHandled;
    FCoords m_lastHover;
};

#endif //FISH_FILLETS_ANDROID_INPUT_H
