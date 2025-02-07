#ifndef FISH_FILLETS_BASEINPUT_H
#define FISH_FILLETS_BASEINPUT_H

#include "common.h"
#include "instance.h"
#include "subsystem/input.h"

class GameScreen;

class BaseInput : public IInputSink {
public:
    BaseInput(Instance& instance, GameScreen& screen) : m_instance(instance), m_screen(screen) { }

    bool keyDown(Key key) override;
    bool pointerDown(FCoords coords) override;

private:
    Instance& m_instance;
    GameScreen& m_screen;
};

#endif //FISH_FILLETS_BASEINPUT_H
