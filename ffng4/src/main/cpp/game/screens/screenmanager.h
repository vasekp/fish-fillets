#ifndef FISH_FILLETS_GAME_STATEMANAGER_H
#define FISH_FILLETS_GAME_STATEMANAGER_H

#include "instance.h"
#include "screen.h"
#include "game/structure/levelrecord.h"
#include "game/graphics/leveltitle.h"

class ScreenManager {
    Instance& m_instance;
    std::unique_ptr<GameScreen> m_screen;
    std::optional<LevelTitle> m_title;
    std::optional<std::chrono::steady_clock::time_point> m_title_hide;
    bool m_options;

public:
    enum class Mode {
        WorldMap,
        Intro,
        Credits
    };

    ScreenManager(Instance& instance) : m_instance(instance), m_options(false) { };

    void refresh();
    void pause();
    void resume();
    void drawFrame();
    bool dispatchMouse(FCoords coords);
    bool dispatchKey(Key key);

    void startMode(Mode mode);
    void announceLevel(const LevelRecord& record);
    void startLevel(const LevelRecord& record);

    bool& options() { return m_options; }

private:
    GameScreen& curScreen();
    void playIntro();
};

#endif //FISH_FILLETS_GAME_STATEMANAGER_H