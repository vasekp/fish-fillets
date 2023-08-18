#ifndef FISH_FILLETS_GAME_IINPUT_H
#define FISH_FILLETS_GAME_IINPUT_H

// always included from input.h

class IInputSource {
public:
    virtual ~IInputSource() { }
    virtual Key pollKey() { return Key::none; }
    virtual void ping() { }
    virtual void reset() { }
    virtual FCoords hover() { return noHover; }

    static constexpr FCoords noHover{-1, -1};
};

inline IInputSource nullInputSource;

class IInputSink {
public:
    virtual bool keyDown(Key key) { return false; }
    virtual bool pointerDown(FCoords coords) { return false; }
    virtual bool pointerMove(FCoords coords) { return false; }
    virtual bool pointerUp(bool empty) { return false; }
    virtual void pointerCancel() { }
    virtual bool longPress(FCoords coords) { return false; }
    virtual bool doubleTap(FCoords coords) { return false; }
    virtual bool twoPointTap() { return false; }
    virtual ~IInputSink() { }
};

#endif //FISH_FILLETS_GAME_IINPUT_H
