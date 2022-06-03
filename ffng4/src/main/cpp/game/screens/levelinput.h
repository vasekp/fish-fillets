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
        std::deque<std::pair<std::chrono::steady_clock::time_point, FCoords>> history;
        ICoords lastDir;
        ICoords lastNonzeroDir;
        Model::Fish fish;
    } m_dirpad;

    enum Buttons {
        bSave,
        bLoad,
        bRestart,
        bExit,
        bOptions,
        bSIZE
    };

    struct Button {
        ogl::Texture texture;
        FCoords coordsFrom;
        FCoords coordsTo;
        Key key;
    };
    std::array<Button, Buttons::bSIZE> m_buttons;
    std::array<bool, Buttons::bSIZE> m_buttonsEnabled;

public:
    enum class ButtonGravity {
        left,
        top
    };

private:
    ButtonGravity m_gravity;
    int m_activeButton;

public:
    LevelInput(Instance& instance);

    void setFish(Model::Fish fish);
    void setSavePossible(bool possible);
    void setLoadPossible(bool possible);

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

    constexpr static float minDistance = 0.1f; // inches
    constexpr static float arrowSize = 0.35f; // inches
    constexpr static float maxButtonSize = 0.35f; // inches
    constexpr static float maxButtonGap = 0.35f; // inches
    constexpr static std::chrono::steady_clock::duration longpressTime = std::chrono::milliseconds(1000);
    constexpr static std::chrono::steady_clock::duration doubletapTime = std::chrono::milliseconds(300);
    constexpr static std::chrono::steady_clock::duration dirpadAppearTime = std::chrono::milliseconds(300);
    constexpr static std::chrono::steady_clock::duration dirpadHistoryLength = std::chrono::milliseconds(100);
    constexpr static std::chrono::steady_clock::time_point absolutePast{};
    constexpr static int noButton = -1;

    constexpr static Color colorSmall{255, 197, 102};
    constexpr static Color colorBig{162, 244, 255};
    constexpr static Color colorButtons{128, 128, 128};
};

#endif //FISH_FILLETS_LEVELINPUT_H
