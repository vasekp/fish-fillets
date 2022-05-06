#ifndef FISH_FILLETS_LEVELSCREEN_H
#define FISH_FILLETS_LEVELSCREEN_H

#include "screen.h"
#include "levelrecord.h"
#include "level.h"
#include "subsystem/audio.h"

class LevelScreen : public GameScreen {
    Level m_level;
    std::multimap<std::string, std::shared_ptr<AudioSource>> m_sounds;
    std::shared_ptr<AudioSource> m_music;
    std::array<float, 3> m_waves;

public:
    LevelScreen(Instance&, const LevelRecord&);

    constexpr static int size_unit = 15;

protected:
    void own_start() override;
    void own_load() override;
    void own_draw() override;

    void create(int width, int height, const std::string& background);
    void setWaves(float amplitude, float period, float speed);
    void addSound(const std::string& name, const std::string& filename);
    void playMusic(const std::string& filename);

    friend class Level;
};

#endif //FISH_FILLETS_LEVELSCREEN_H