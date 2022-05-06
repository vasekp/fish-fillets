#ifndef FISH_FILLETS_GAME_TESTSCREEN_H
#define FISH_FILLETS_GAME_TESTSCREEN_H

#include "screen.h"

class TestScreen : public GameScreen {
    std::shared_ptr<AudioSource> m_music;
    std::shared_ptr<AudioSource> m_sound[6];

public:
    TestScreen(Instance&, const char*, const char*, const char*);

private:
    void own_load() override;
    void own_draw() override;
    bool own_mouse(unsigned, unsigned) override;
};

#endif //FISH_FILLETS_GAME_TESTSCREEN_H