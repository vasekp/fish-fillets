#include "levels.h"

Levels::Levels(Instance& instance) : m_levels(), m_script(instance, *this) {
    m_script.registerFn("file_include", file_include);
    m_script.registerFn("branch_addNode", branch_addNode);
    m_script.registerFn("branch_setEnding", branch_setEnding);
    m_script.registerFn("worldmap_addDesc", worldmap_addDesc);
    m_script.registerFn("node_bestSolution", node_bestSolution);
    m_script.loadFile("override/worldmap.lua");
}

int Levels::file_include(lua_State* L) {
    auto [filename] = lua::args<lua::types::string>(L);
    Script::from(L).loadFile(filename);
    return 0;
}

int Levels::branch_addNode(lua_State* L) {
    auto [parent, codename, filename, x, y, color, ending] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string,
            lua::types::integer,
            lua::types::integer,
            lua::optional<lua::types::integer, LevelRecord::no_color>,
            lua::optional<lua::types::string>
    >(L);
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    std::shared_ptr<LevelRecord> parentRecord = *parent
            ? self.m_levels.at(parent)
            : decltype(parentRecord){};
    self.m_levels.insert({codename, std::make_shared<LevelRecord>(parentRecord, filename, ending, false, Coords{x, y}, color)});
    return 0;
}

int Levels::branch_setEnding(lua_State* L) {
    auto [codename, filename, ending] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string
    >(L);
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    self.m_finale = std::make_shared<LevelRecord>(std::shared_ptr<LevelRecord>(), filename, ending, false, Coords{}, LevelRecord::no_color);
    return 0;
}

int Levels::worldmap_addDesc(lua_State* L) {
    auto [codename, lang, levelname, branch] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string,
            lua::types::string
    >(L);
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    if(self.m_levels.contains(codename))
        self.m_levels.at(codename)->description[lang] = levelname;
    return 0;
}

int Levels::node_bestSolution(lua_State* L) {
    auto [codename, moves, name] = lua::args<
            lua::types::string,
            lua::types::integer,
            lua::types::string
    >(L);
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    if(self.m_levels.contains(codename))
        self.m_levels.at(codename)->best = {name, moves};
    return 0;
}