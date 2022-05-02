#ifndef FISH_FILLETS_WORLDMAP_H
#define FISH_FILLETS_WORLDMAP_H

#include "screen.h"

class WorldMap : public GameScreen {
    bool m_loadingFrame;

public:
    WorldMap(Instance*);
    void prep_loading() { m_loadingFrame = true; }

    enum class MaskColors : uint32_t {
        exit = 0x008080FF
    };

private:
    void own_load() override;
    void own_draw() override;
};

#endif //FISH_FILLETS_WORLDMAP_H