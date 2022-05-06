#ifndef FISH_FILLETS_LEVELS_H
#define FISH_FILLETS_LEVELS_H

#include "instance.h"
#include "levelrecord.h"
#include "subsystem/script.h"

class Levels : public ScriptReferrer {
    std::map<std::string, std::shared_ptr<LevelRecord>> m_levels;
    std::shared_ptr<LevelRecord> m_finale;
    Script m_script;

public:
    Levels(Instance&);

    auto begin() const { return m_levels.cbegin(); }
    auto end() const { return m_levels.cend(); }

private:
    static int branch_addNode(lua_State*);
    static int branch_setEnding(lua_State*);
    static int worldmap_addDesc(lua_State*);
    static int node_bestSolution(lua_State*);
};

#endif //FISH_FILLETS_LEVELS_H