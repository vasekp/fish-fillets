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
        ignore,
        button
    };

    struct {
        DirpadState state;
        std::chrono::steady_clock::time_point touchTime;
        FCoords refPos;
        ICoords lastDir;
        Model::Fish fish;
    } m_dirpad;

    struct Button {
        ogl::Texture texture;
        FCoords coordsFrom;
        FCoords coordsTo;
        Key key;
    };
    std::array<Button, 5> m_buttons;

    int m_activeButton;

public:
    enum class ButtonGravity {
        left,
        top
    };

private:
    ButtonGravity m_gravity;

public:
    LevelInput(Instance& instance);

    void setFish(Model::Fish fish);

    bool handleKeyDown(Key key) override;
    bool handleKeyUp(Key key) override;
    bool handlePointerDown(FCoords pos) override;
    bool handlePointerUp() override;
    bool handlePointerMove(FCoords pos) override;

    void refresh();
    void draw(const DrawTarget& target);
    Key pool();
    void setButtonGravity(ButtonGravity gravity);
    FCoords getReserve();

private:
    static unsigned index(Key key);
    static Key toKey(ICoords dir);
    int findButton(FCoords pos);

    void drawButtons(const DrawTarget& target);
    void drawDirpad(const DrawTarget& target);

    constexpr static float minDistance = 0.25f; // inches
    constexpr static float maxButtonSize = 0.35f; // inches
    constexpr static float maxButtonGap = 0.35f; // inches
    constexpr static std::chrono::steady_clock::duration longpressTime = std::chrono::milliseconds (1000);
    constexpr static std::chrono::steady_clock::duration doubletapTime = std::chrono::milliseconds (300);
    constexpr static std::chrono::steady_clock::duration dirpadAppearTime = std::chrono::milliseconds (300);
    constexpr static std::chrono::steady_clock::time_point absolutePast{};
    constexpr static int noButton = -1;

    constexpr static Color colorSmall{255, 197, 102};
    constexpr static Color colorBig{162, 244, 255};
    constexpr static Color colorButtons{128, 128, 128};
};

#endif //FISH_FILLETS_LEVELINPUT_H
