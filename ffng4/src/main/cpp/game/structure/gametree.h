#ifndef FISH_FILLETS_GAMETREE_H
#define FISH_FILLETS_GAMETREE_H

#include "instance.h"
#include "levelrecord.h"
#include "subsystem/script.h"

class GameTree : public ScriptReferrer {
    std::map<std::string, std::shared_ptr<LevelRecord>> m_levels;
    std::shared_ptr<LevelRecord> m_finale;
    Script m_script;

public:
    GameTree(Instance &);

    auto begin() const { return m_levels.cbegin(); }

    auto end() const { return m_levels.cend(); }

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