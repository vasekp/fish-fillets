#ifndef FISH_FILLETS_LEVELSCREEN_H
#define FISH_FILLETS_LEVELSCREEN_H

#include "screen.h"
#include "levelrecord.h"
#include "level.h"
#include "subsystem/audio.h"
#include "timer.h"


class LevelScreen : public GameScreen {
    Level m_level;
    std::multimap<std::string, AudioSource> m_sounds;
    AudioSource m_music;
    std::array<float, 3> m_waves;
    Timer m_timer;

public:
    LevelScreen(Instance&, const LevelRecord&);

    constexpr static int size_unit = 15;

protected:
    void own_start() override;
    void own_refresh() override;
    void own_pause() override;
    void own_resume() override;
    void own_draw(const DrawTarget& target) override;

    void create(int width, int height, const std::string& background);
    void setWaves(float amplitude, float period, float speed);
    AudioSource& addSound(const std::string& name, const std::string& filename, bool single = false);
    void playMusic(const std::string& filename);
    void stopMusic();

    bool own_mouse(unsigned int x, unsigned int y) override;

    friend class Level;
};

#endif //FISH_FILLETS_LEVELSCREEN_H