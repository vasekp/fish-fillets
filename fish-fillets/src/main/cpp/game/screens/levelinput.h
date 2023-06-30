#ifndef FISH_FILLETS_LEVELINPUT_H
#define FISH_FILLETS_LEVELINPUT_H

#include "common.h"
#include "subsystem/input.h"
#include "game/logic/model.h"

class Instance;
class DrawTarget;
class LevelScreen;

class LevelInput : public IInputSink {
    Instance& m_instance;
    LevelScreen& m_screen;
    Model::Fish m_activeFish;

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
        bool inside;
    } m_dirpad;

    struct Button {
        TextImage image;
        FCoords coordsFrom;
        FCoords coordsTo;
        Key key;
        bool enabled = true;
        bool flashing = false;
        float flashTime = 0.f;
    };
    std::unique_ptr<IFont> m_buttonsFont;
    std::vector<Button> m_buttons;

    PNGImage m_fishSmall;
    PNGImage m_fishBig;

    int m_activeButton;

public:
    LevelInput(Instance& instance, LevelScreen& screen);

    void setFish(Model::Fish fish);
    void setSavePossible(bool possible);
    void setLoadPossible(bool possible);
    void setRestartPossible(bool possible);
    void flashButton(Key which);

    bool keyDown(Key key) override;
    bool pointerDown(FCoords coords) override;
    bool pointerMove(FCoords coords) override;
    bool pointerUp(bool empty) override;
    void pointerCancel() override;
    bool longPress(FCoords coords) override;
    bool doubleTap(FCoords coords) override;
    bool twoPointTap() override;

    void resize();
    void draw(const DrawTarget& target, float time);
    Key pool();

private:
    int findButton(FCoords pos);
    Button& keyButton(Key key);

    void drawButtons(const DrawTarget& target, float time);
    void drawDirpad(const DrawTarget& target);

    constexpr static const char* fontFilename = "font/FFArrows.ttf";
    constexpr static float minDistance = 40.f; // all dimension in "base pixels"
    constexpr static float buttonDistance = 35.f;
    constexpr static float arrowSize = 64.f;
    constexpr static float buttonSize = 48.f;
    constexpr static float buttonFontSize = 40.f;
    constexpr static std::chrono::steady_clock::duration dirpadAppearTime = std::chrono::milliseconds(300);
    constexpr static std::chrono::steady_clock::duration dirpadHistoryLength = std::chrono::milliseconds(100);
    constexpr static std::chrono::steady_clock::time_point absolutePast{};
    constexpr static int noButton = -1;

    constexpr static Color colorSmall{255, 197, 102};
    constexpr static Color colorBig{162, 244, 255};
    constexpr static Color colorButtons{128, 128, 128};
    constexpr static float flashDuration = 0.5f;
    constexpr static float flashBrightness = 1.5f;
};

#endif //FISH_FILLETS_LEVELINPUT_H
