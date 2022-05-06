#ifndef FISH_FILLETS_GAME_WORLDMAP_H
#define FISH_FILLETS_GAME_WORLDMAP_H

#include "screen.h"
#include "game/levels.h"

class WorldMap : public GameScreen {
    enum class Frames {
        none,
        loading,
        exit,
        options,
        intro,
        credits
    };

    struct Branch {
        std::string start;
        Color maskColor;
    };

    std::shared_ptr<AudioSource> m_music;
    std::vector<Image> nodeImages;
    std::map<Frames, Color> m_maskColors;
    std::vector<std::shared_ptr<LevelRecord>> m_open;
    std::vector<std::shared_ptr<LevelRecord>> m_forks;
    Frames m_nextFrame;

    static constexpr int nodeRadius = 9;
    static constexpr int nodeTolerance = 15;

public:
    struct MaskColors {
        static constexpr Color exit = 0x008080;
        static constexpr Color options = 0x008000;
        static constexpr Color intro = 0x000080;
        static constexpr Color credits = 0x808000;
        static constexpr Color mainBranch = 0xFFFFFF;
    };

    WorldMap(Instance&);
    void staticFrame(Frames frame);

private:
    void own_start() override;
    void own_load() override;
    void own_draw() override;
    bool own_mouse(unsigned, unsigned) override;

    void drawMasked(Color c);
    void refresh();
};

#endif //FISH_FILLETS_GAME_WORLDMAP_H