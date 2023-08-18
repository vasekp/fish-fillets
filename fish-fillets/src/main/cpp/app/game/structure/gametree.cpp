#include "gametree.h"
#include "subsystem/files.h"

GameTree::GameTree(Instance& instance) :
        m_instance(instance),
        m_levels(),
        m_script(instance, *this)
{
    m_script.registerFn("map_node", lua::wrap<&GameTree::map_node>);
    m_script.registerFn("map_branch", lua::wrap<&GameTree::map_branch>);
    m_script.registerFn("map_ending", lua::wrap<&GameTree::map_ending>);
    m_script.loadFile("script/worldmap.lua");
}

void GameTree::addNode(const std::string& codename, const std::string& parent, const std::string& filename,
        int x, int y, int depth, int maskColor,
        std::map<std::string, std::string>&& title, const std::optional<std::string>& ending) {
    LevelRecord* parentRecord = parent.empty() ? nullptr : &m_levels.at(parent);
    if(!depth)
        depth = parentRecord ? parentRecord->depth + 1 : 1;
    LevelRecord newRecord{codename, std::move(title), parentRecord, FCoords{x, y}, depth, filename, ending.value_or(""), maskColor};
    newRecord.solved = m_instance.files().user(newRecord.solveFilename())->exists();
    m_levels.insert({codename, std::move(newRecord)});
}

void GameTree::map_node(const std::string& codename, const std::string& parent, const std::string& filename,
        int x, int y, std::map<std::string, std::string>&& title, const std::optional<std::string>& ending) {
    addNode(codename, parent, filename, x, y, 0, LevelRecord::noColor, std::move(title), ending);
}

void GameTree::map_branch(const std::string& codename, const std::string& parent, const std::string& filename,
        int x, int y, int maskColor, std::map<std::string, std::string>&& title) {
    addNode(codename, parent, filename, x, y, 0, maskColor, std::move(title), std::optional<std::string>{});
}

void GameTree::map_ending(const std::string& codename, const std::string& filename,
        int x, int y, int maskColor, std::map<std::string, std::string>&& title, const std::string& ending) {
    addNode(codename, "", filename, x, y, LevelRecord::depthEnding, maskColor, std::move(title), ending);
}
