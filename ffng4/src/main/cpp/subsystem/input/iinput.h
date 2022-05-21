#ifndef FFNG_GAME_IINPUT_H
#define FFNG_GAME_IINPUT_H

#include "../../common.h"
#include "key.h"

class Instance;
class DrawTarget;

class IInput {
public:
    virtual bool handleKeyDown(Key key) = 0;
    virtual bool handleKeyUp(Key key) = 0;
    virtual bool handlePointerDown(FCoords pos) = 0;
    virtual bool handlePointerUp() = 0;
    virtual bool handlePointerMove(FCoords pos) = 0;
};

#endif //FFNG_GAME_IINPUT_H
