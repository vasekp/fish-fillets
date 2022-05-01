#ifndef FISH_FILLETS_WORLDMAP_H2
#define FISH_FILLETS_WORLDMAP_H2

#include "screen.h"

class TestScreen : public GameScreen {
    AudioSource music;

public:
    TestScreen(Instance*, const char*, const char*);

private:
    void own_draw() override;
};

#endif //FISH_FILLETS_WORLDMAP_H2
