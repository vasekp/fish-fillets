#ifndef FISH_FILLETS_BASEINPUT_H
#define FISH_FILLETS_BASEINPUT_H

#include "subsystem/input.h"

class GameScreen;

class BaseInput : public IInputSink {
    Instance& m_instance;
    GameScreen& m_screen;

public:
    BaseInput(Instance& instance, GameScreen& screen) : m_instance(instance), m_screen(screen) { }

    bool keyDown(Key key) override;
    bool pointerDown(FCoords coords) override;
    bool doubleTap(FCoords coords) override;
};

#endif //FISH_FILLETS_BASEINPUT_H
