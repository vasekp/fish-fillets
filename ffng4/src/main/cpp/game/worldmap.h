#ifndef FISH_FILLETS_GAME_WORLDMAP_H
#define FISH_FILLETS_GAME_WORLDMAP_H

#include "screen.h"
#include "levelinfo.h"

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

    Frames m_nextFrame;
    std::vector<Image> nodeImages;
    std::map<Frames, Color> m_maskColors;
    std::map<std::string, LevelInfo> m_levels;
    std::vector<Branch> m_branches;
    std::vector<LevelInfo> m_open;

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
    void own_load() override;
    void own_draw() override;
    bool own_mouse(unsigned, unsigned) override;

    void drawMasked(Color c);

    static int file_include(lua_State*);
    static int branch_addNode(lua_State*);
    static int branch_setEnding(lua_State*);
    static int worldmap_addDesc(lua_State*);
    static int node_bestSolution(lua_State*);
};

#endif //FISH_FILLETS_GAME_WORLDMAP_H