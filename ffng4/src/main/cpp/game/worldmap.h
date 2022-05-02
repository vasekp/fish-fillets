#ifndef FISH_FILLETS_WORLDMAP_H
#define FISH_FILLETS_WORLDMAP_H

#include "screen.h"

class WorldMap : public GameScreen {
public:
    struct MaskColors {
        static constexpr Color exit = 0x008080FF;
        static constexpr Color options = 0x008000FF;
        static constexpr Color intro = 0x000080FF;
        static constexpr Color credits = 0x808000FF;
        static constexpr Color mainBranch = 0xFFFFFFFF;
    };

    enum class Frames {
        none,
        loading,
        exit,
        options,
        intro,
        credits
    };

    WorldMap(Instance*);
    void staticFrame(Frames frame);

private:
    Frames m_nextFrame;

    void own_load() override;
    void own_draw() override;
    bool own_mouse(unsigned, unsigned) override;

    std::map<Frames, Color> maskColors;
    void drawMasked(Color c);

    static constexpr int nodeRadius = 9;
    std::vector<std::shared_ptr<Image>> nodeImages;
};

#endif //FISH_FILLETS_WORLDMAP_H