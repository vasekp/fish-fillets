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
    static void branch_addNode(lua_State* L,
                               const std::string& parent, const std::string& codename, const std::string& filename,
                               int x, int y, lua::optional<int, -1> color, const lua::optional<std::string>& ending);
    static void branch_setEnding(lua_State* L, const std::string& codename, const std::string& filename, const std::string& ending);
    static void worldmap_addDesc(lua_State* L, const std::string& codename, const std::string& lang, const std::string& levelname, const std::string& branch);
    static void node_bestSolution(lua_State* L, const std::string& codename, int moves, const std::string& name);
};

#endif //FISH_FILLETS_LEVELS_H