#ifndef FISH_FILLETS_GAMETREE_H
#define FISH_FILLETS_GAMETREE_H

#include "instance.h"
#include "levelrecord.h"
#include "subsystem/script.h"

class GameTree : public ScriptReferrer {
    Instance& m_instance;
    std::map<std::string, LevelRecord> m_levels;
    Script m_script;

public:
    GameTree(Instance& instance);

    auto begin() { return m_levels.begin(); }
    auto end() { return m_levels.end(); }

private:
    void branch_addNode(const std::string &parent, const std::string &codename, const std::string &filename,
            int x, int y, std::optional<int> color, const std::optional<std::string> &ending);
    void branch_setEnding(const std::string &codename, const std::string &filename,
                          const std::string &ending);
    void worldmap_addDesc(const std::string &codename, const std::string &lang,
                          const std::string &levelname, const std::string &branch);
    void node_bestSolution(const std::string &codename, int moves, const std::string &name);
};

#endif //FISH_FILLETS_GAMETREE_H