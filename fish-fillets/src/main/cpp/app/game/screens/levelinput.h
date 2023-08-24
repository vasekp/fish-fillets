#ifndef FISH_FILLETS_LEVELINPUT_H
#define FISH_FILLETS_LEVELINPUT_H

#include "common.h"
#include "instance.h"
#include "subsystem/input.h"
#include "subsystem/graphics/image.h"
#include "game/logic/model.h"

class DrawTarget;
class LevelScreen;

class LevelInput : public IInputSink {
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
    bool pointerUp() override;
    void pointerCancel() override;
    bool twoPointTap() override;

    void resize();
    void update() override;
    void draw(DrawTarget& target);
    Key poll();

private:
    Instance& m_instance;
    LevelScreen& m_screen;
    Model::Fish m_activeFish;

    enum class DirpadState {
        idle,
        wait,
        follow,
        ignore,
        button,
        goTo
    };

    struct {
        DirpadState state;
        std::chrono::steady_clock::time_point touchTime;
        std::deque<std::pair<std::chrono::steady_clock::time_point, FCoords>> history;
        FCoords gotoPos;
        ICoords lastDir;
        ICoords lastNonzeroDir;
    } m_dirpad;
    bool m_pointerAction;

    struct Button {
        ImageRef image;
        FCoords coordsFrom;
        FCoords coordsTo;
        Key key;
        bool enabled = true;
        bool flashing = false;
        std::chrono::steady_clock::time_point flashTime;
        float alpha;
    };
    std::unique_ptr<IFont> m_buttonsFont;
    std::vector<Button> m_buttons;
    const Button* m_activeButton;

    ImageRef m_fishSmall;
    ImageRef m_fishBig;

    const Button* findButton(FCoords pos);
    Button& keyButton(Key key);

    void drawButtons(DrawTarget& target);
    void drawDirpad(DrawTarget& target);
};

#endif //FISH_FILLETS_LEVELINPUT_H
