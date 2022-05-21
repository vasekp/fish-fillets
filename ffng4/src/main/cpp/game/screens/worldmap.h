#ifndef FISH_FILLETS_GAME_WORLDMAP_H
#define FISH_FILLETS_GAME_WORLDMAP_H

#include "screen.h"
#include "game/structure/gametree.h"

class WorldMap : public GameScreen {
    enum class Frames {
        none,
        loading,
        exit,
        options,
        intro,
        credits
    };

    BaseInput m_input;
    AudioSourceRef m_music;
    std::vector<const Image*> m_nodeImages;
    std::map<Frames, Color> m_maskColors;
    std::vector<std::shared_ptr<LevelRecord>> m_open;
    std::vector<std::shared_ptr<LevelRecord>> m_forks;
    Frames m_staticFrame;

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

    IInput& input() override { return m_input; }

protected:
    void own_start() override;
    void own_resume() override;
    void own_refresh() override;
    void own_draw(const DrawTarget& target, float dt) override;
    bool own_mouse(unsigned, unsigned) override;

private:
    void drawMasked(const DrawTarget& target, Color c);
};

#endif //FISH_FILLETS_GAME_WORLDMAP_H