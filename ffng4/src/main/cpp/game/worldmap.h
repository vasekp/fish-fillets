#ifndef FISH_FILLETS_WORLDMAP_H
#define FISH_FILLETS_WORLDMAP_H

#include "screen.h"

class WorldMap : public GameScreen {
    AudioSource music;

public:
    WorldMap(Instance*);

private:
    void own_draw() override;
};

#endif //FISH_FILLETS_WORLDMAP_H
