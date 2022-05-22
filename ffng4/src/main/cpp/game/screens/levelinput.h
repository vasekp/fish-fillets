#ifndef FISH_FILLETS_LEVELINPUT_H
#define FISH_FILLETS_LEVELINPUT_H

#include "common.h"
#include "subsystem/input.h"
#include "game/logic/model.h"

class Instance;
class DrawTarget;

class LevelInput : public IInput {
    Instance& m_instance;
    Key m_lastKey;

    enum class DirpadState {
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
        Model::Fish fish;
    } m_dirpad;
    float m_density;

public:
    LevelInput(Instance& instance);

    void setDensity(float density);
    void setFish(Model::Fish fish);

    bool handleKeyDown(Key key) override;
    bool handleKeyUp(Key key) override;
    bool handlePointerDown(FCoords pos) override;
    bool handlePointerUp() override;
    bool handlePointerMove(FCoords pos) override;

    void refresh();
    void draw(const DrawTarget& target);
    Key pool();

private:
    static unsigned index(Key key);
    static Key toKey(ICoords dir);

    constexpr static float minDistance = 0.3f; // inches
    constexpr static std::chrono::steady_clock::duration longpressTime = std::chrono::milliseconds (1000);
    constexpr static std::chrono::steady_clock::duration doubletapTime = std::chrono::milliseconds (300);
    constexpr static std::chrono::steady_clock::duration dirpadAppearTime = std::chrono::milliseconds (300);
    constexpr static std::chrono::steady_clock::time_point absolutePast{};

    constexpr static Color colorSmall{255, 197, 102};
    constexpr static Color colorBig{162, 244, 255};
};

#endif //FISH_FILLETS_LEVELINPUT_H
