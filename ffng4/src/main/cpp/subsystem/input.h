#ifndef FISH_FILLETS_INPUT_H
#define FISH_FILLETS_INPUT_H

#include "common.h"
#include "subsystem/input/key.h"

class Instance;

class Input {
    Instance& m_instance;
    Key m_lastKey;

    enum class DirpadState {
        disabled,
        idle,
        wait,
        follow,
        ignore
    };

    struct {
        DirpadState state;
        std::chrono::steady_clock::time_point touchTime;
        FCoords refPos;
        ICoords lastDir;
    } m_dirpad;
    float m_density;

public:
    Input(Instance& instance);

    void setDensity(float density);

    bool handleKeyDown(Key key);
    bool handleKeyUp(Key key);
    bool handlePointerDown(FCoords pos);
    bool handlePointerUp();
    bool handlePointerMove(FCoords pos);
    Key pool();

private:
    static unsigned index(Key key);
    static Key toKey(ICoords dir);

    constexpr static float minDistance = 0.5f; // inches
    constexpr static std::chrono::steady_clock::duration longpressTime = std::chrono::milliseconds (1000);
    constexpr static std::chrono::steady_clock::duration doubletapTime = std::chrono::milliseconds (500);
    constexpr static std::chrono::steady_clock::time_point off{};
};

#endif //FISH_FILLETS_INPUT_H