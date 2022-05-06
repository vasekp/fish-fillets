#ifndef FISH_FILLETS_LEVELS_H
#define FISH_FILLETS_LEVELS_H

#include "instance.h"
#include "subsystem/script.h"

enum class LevelState {
    locked,
    open,
    solved
};

struct LevelRecord {
    std::shared_ptr<LevelRecord> parent;
    std::string filename;
    bool solved;

    Coords coords;
    int maskColor;

    LevelRecord(std::shared_ptr<LevelRecord>, std::string, bool, Coords, int);
    LevelState state() const;

    constexpr static int no_color = -1;
};

class Levels : public ScriptReferrer {
    std::map<std::string, std::shared_ptr<LevelRecord>> m_levels;
    Script m_script;

public:
    Levels(Instance&);

    auto begin() const { return m_levels.cbegin(); }
    auto end() const { return m_levels.cend(); }

private:
    static int file_include(lua_State*);
    static int branch_addNode(lua_State*);
    static int branch_setEnding(lua_State*);
    static int worldmap_addDesc(lua_State*);
    static int node_bestSolution(lua_State*);
};

#endif //FISH_FILLETS_LEVELS_H