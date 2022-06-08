#ifndef FFNG_GAME_IINPUT_H
#define FFNG_GAME_IINPUT_H

#include "common.h"
#include "key.h"

class Instance;

class IInputProvider {
public:
    virtual Key poolKey() { return Key::none; }
    virtual void reset() { }
    virtual ~IInputProvider() { }
};

class IInputSink {
public:
    virtual bool keyDown(Key key) { return false; }
    virtual bool keyUp(Key key) { return false; }
    virtual bool pointerDown(FCoords coords) { return false; }
    virtual bool pointerMove(FCoords coords) { return false; }
    virtual bool pointerUp() { return false; }
    virtual bool longPress(FCoords coords) { return false; }
    virtual bool doubleTap(FCoords coords) { return false; }
    virtual ~IInputSink() { }
};

#endif //FFNG_GAME_IINPUT_H
