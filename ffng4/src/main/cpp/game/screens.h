#ifndef FISH_FILLETS_GAME_STATEMANAGER_H
#define FISH_FILLETS_GAME_STATEMANAGER_H

#include "instance.h"
#include "screen.h"
#include "levelrecord.h"
#include "title.h"

class Screens {
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

    Screens(Instance& instance) : m_instance(instance), m_options(false) { };

    void refresh();
    void pause();
    void resume();
    void drawFrame();
    bool mouse(Coords coords);

    void startMode(Mode mode);
    void announceLevel(const LevelRecord& record);
    void startLevel(const LevelRecord& record);

    bool& options() { return m_options; }

private:
    GameScreen& curScreen();
    void playIntro();
};

#endif //FISH_FILLETS_GAME_STATEMANAGER_H