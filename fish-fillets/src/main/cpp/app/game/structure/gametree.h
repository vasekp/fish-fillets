#ifndef FISH_FILLETS_GAMETREE_H
#define FISH_FILLETS_GAMETREE_H

#include "instance.h"
#include "levelrecord.h"
#include "subsystem/script.h"

class GameTree : public ScriptReferrer {
public:
    GameTree(Instance& instance);

    auto begin() { return m_levels.begin(); }
    auto end() { return m_levels.end(); }

private:
    Instance& m_instance;
    std::map<std::string, LevelRecord> m_levels;
    Script m_script;

    void map_node(const std::string& codename, const std::string& parent, const std::string& filename,
            int x, int y, std::map<std::string, std::string>&& title, const std::optional<std::string>& ending);
    void map_branch(const std::string& codename, const std::string& parent, const std::string& filename,
            int x, int y, int maskColor, std::map<std::string, std::string>&& title);
    void map_ending(const std::string& codename, const std::string& filename,
            int x, int y, int maskColor, std::map<std::string, std::string>&& title, const std::string& ending);

    void addNode(const std::string& codename, const std::string& parent, const std::string& filename,
        int x, int y, int depth, int maskColor, std::map<std::string, std::string>&& title,
        const std::optional<std::string>& ending);
};

#endif //FISH_FILLETS_GAMETREE_H
