#include "levels.h"

Levels::Levels(Instance& instance) : m_levels(), m_script(instance, *this) {
    m_script.registerFn("branch_addNode", lua::wrap<branch_addNode>);
    m_script.registerFn("branch_setEnding", lua::wrap<branch_setEnding>);
    m_script.registerFn("worldmap_addDesc", lua::wrap<worldmap_addDesc>);
    m_script.registerFn("node_bestSolution", lua::wrap<node_bestSolution>);
    m_script.loadFile("override/worldmap.lua");
}

void Levels::branch_addNode(lua_State* L, const std::string& parent, const std::string& codename, const std::string& filename,
                            int x, int y, std::optional<int> color, const std::optional<std::string>& ending) {
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    std::shared_ptr<LevelRecord> parentRecord = parent.empty()
            ? decltype(parentRecord){}
            : self.m_levels.at(parent);
    self.m_levels.insert({codename, std::make_shared<LevelRecord>(parentRecord, filename, ending.value_or(""), false, Coords{x, y}, color.value_or(-1))});
}

void Levels::branch_setEnding(lua_State* L, const std::string& codename, const std::string& filename, const std::string& ending) {
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    self.m_finale = std::make_shared<LevelRecord>(std::shared_ptr<LevelRecord>(), filename, ending, false, Coords{}, LevelRecord::no_color);
}

void Levels::worldmap_addDesc(lua_State* L, const std::string& codename, const std::string& lang, const std::string& levelname, const std::string& branch) {
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    if(self.m_levels.contains(codename))
        self.m_levels.at(codename)->description[lang] = levelname;
}

void Levels::node_bestSolution(lua_State* L, const std::string& codename, int moves, const std::string& name) {
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    if(self.m_levels.contains(codename))
        self.m_levels.at(codename)->best = {name, moves};
}