#ifndef FISH_FILLETS_LEVELSCREEN_H
#define FISH_FILLETS_LEVELSCREEN_H

#include "screen.h"
#include "game/structure/levelrecord.h"
#include "game/logic/level.h"
#include "subsystem/audio.h"
#include "game/logic/timer.h"
#include "game/graphics/subtitles.h"

class LevelScreen : public GameScreen {
    Level m_level;
    std::multimap<std::string, AudioSource> m_sounds;
    AudioSource m_music;
    std::array<float, 3> m_waves;
    Timer m_timer;
    Subtitles m_subs;
    bool m_fullLoad;

public:
    LevelScreen(Instance&, const LevelRecord&);

    constexpr static int size_unit = 15;

protected:
    void own_start() override;
    void own_refresh() override;
    void own_pause() override;
    void own_resume() override;
    void own_draw(const DrawTarget& target, float dt) override;

    void create(int width, int height, const std::string& background);
    void setWaves(float amplitude, float period, float speed);
    AudioSource& addSound(const std::string& name, const std::string& filename, bool single = false);
    void addSubtitle(const std::string& text, const std::string& color);
    void playMusic(const std::string& filename);
    void stopMusic();

    bool own_mouse(unsigned int x, unsigned int y) override;

    friend class Level;
};

#endif //FISH_FILLETS_LEVELSCREEN_H