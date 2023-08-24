#ifndef FISH_FILLETS_GAME_IINPUT_H
#define FISH_FILLETS_GAME_IINPUT_H

// always included from input.h

class IInputSource {
public:
    virtual ~IInputSource() { }
    virtual Key pollKey() { return Key::none; }
    virtual void reset() { }
    virtual FCoords hover() { return noHover; }

    static constexpr FCoords noHover{-1, -1};
};

inline IInputSource nullInputSource;

class IInputSink {
public:
    virtual bool keyDown(Key key) { return false; }
    virtual bool pointerDown(FCoords coords) { return false; }
    virtual void pointerMove(FCoords coords) { }
    virtual bool pointerUp() { return false; }
    virtual void pointerCancel() { }
    virtual bool twoPointTap() { return false; }
    virtual void update() { }
    virtual ~IInputSink() { }
};

#endif //FISH_FILLETS_GAME_IINPUT_H
