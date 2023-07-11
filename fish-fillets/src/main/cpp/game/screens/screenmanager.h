#ifndef FISH_FILLETS_GAME_SCREENMANAGER_H
#define FISH_FILLETS_GAME_SCREENMANAGER_H

#include "instance.h"
#include "gamescreen.h"
#include "leveltitle.h"
#include "options.h"

class LevelRecord;

class ScreenManager {
    Instance& m_instance;
    std::unique_ptr<GameScreen> m_screen;
    std::unique_ptr<GameScreen> m_next;
    LevelTitle m_title;
    OptionsOverlay m_options;

public:
    enum class Mode {
        WorldMap,
        Intro,
        Credits
    };

    ScreenManager(Instance& instance) : m_instance(instance), m_title(instance), m_options(instance) { };

    void resize();
    void pause();
    void resume();
    void drawFrame();

    void startMode(Mode mode);
    void announceLevel(const std::string& title);
    void startLevel(LevelRecord& record, bool replay = false);
    void poster(const LevelRecord& record);

    OptionsOverlay& options() { return m_options; }

    GameScreen& curScreen() { return *m_screen; }
    IInputSink& input();
    IInputSource& inputSourceMasked();

private:
    template<class ScreenType, typename... Ts>
    ScreenType& open(Ts&&... ts);

    void useNext();
};

#endif //FISH_FILLETS_GAME_SCREENMANAGER_H
