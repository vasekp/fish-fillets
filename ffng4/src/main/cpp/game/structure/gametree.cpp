#include "gametree.h"

GameTree::GameTree(Instance& instance) : m_levels(), m_script(instance, *this) {
    m_script.registerFn("branch_addNode", lua::wrap<&GameTree::branch_addNode>);
    m_script.registerFn("branch_setEnding", lua::wrap<&GameTree::branch_setEnding>);
    m_script.registerFn("worldmap_addDesc", lua::wrap<&GameTree::worldmap_addDesc>);
    m_script.registerFn("node_bestSolution", lua::wrap<&GameTree::node_bestSolution>);
    m_script.loadFile("script/worldmap.lua");
}

void GameTree::branch_addNode(const std::string& parent, const std::string& codename, const std::string& filename,
                              int x, int y, std::optional<int> color, const std::optional<std::string>& ending) {
    LOGD("addNode %s -> %s", parent.c_str(), codename.c_str());
    std::shared_ptr<LevelRecord> parentRecord = parent.empty()
            ? decltype(parentRecord){}
            : m_levels.at(parent);
    int depth = parentRecord ? parentRecord->depth + 1 : 1;
    m_levels.insert({codename, std::make_shared<LevelRecord>(parentRecord, filename, ending.value_or(""), depth, false, FCoords{x, y}, color.value_or(-1))});
    LOGD("addNode %s -> %s done", parent.c_str(), codename.c_str());
}

void GameTree::branch_setEnding(const std::string& codename, const std::string& filename, const std::string& ending) {
    m_finale = std::make_shared<LevelRecord>(std::shared_ptr<LevelRecord>(), filename, ending, 1, false, FCoords{}, LevelRecord::no_color);
}

void GameTree::worldmap_addDesc(const std::string& codename, const std::string& lang, const std::string& levelname, const std::string& branch) {
    if(m_levels.contains(codename))
        m_levels.at(codename)->description[lang] = levelname;
}

void GameTree::node_bestSolution(const std::string& codename, int moves, const std::string& name) {
    if(m_levels.contains(codename))
        m_levels.at(codename)->best = {name, moves};
}