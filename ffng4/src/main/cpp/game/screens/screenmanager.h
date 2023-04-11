#ifndef FISH_FILLETS_GAME_STATEMANAGER_H
#define FISH_FILLETS_GAME_STATEMANAGER_H

#include "instance.h"
#include "screen.h"
#include "game/logic/level.h"
#include "game/graphics/leveltitle.h"

class ScreenManager {
    Instance& m_instance;
    std::unique_ptr<GameScreen> m_screen;
    std::unique_ptr<GameScreen> m_next;
    LevelTitle m_title;
    bool m_options;

public:
    enum class Mode {
        WorldMap,
        Intro,
        Credits
    };

    ScreenManager(Instance& instance) : m_instance(instance), m_title(instance), m_options(false) { };

    void resize();
    void pause();
    void resume();
    void drawFrame();

    void startMode(Mode mode);
    void announceLevel(const std::string& title);
    Level& startLevel(LevelRecord& record);

    template<class ScreenType, typename... Ts>
    ScreenType& open(Ts&&... ts);

    GameScreen& curScreen() { return *m_screen; }
    bool& options() { return m_options; }

private:
    void playIntro();
};

#endif //FISH_FILLETS_GAME_STATEMANAGER_H
