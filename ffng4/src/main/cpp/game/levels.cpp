#include "levels.h"

LevelRecord::LevelRecord(std::shared_ptr<LevelRecord> parent_, std::string filename_, bool solved_,
                         Coords coords_, int maskColor_) :
    parent(std::move(parent_)),
    filename(std::move(filename_)),
    solved(solved_),
    coords(coords_),
    maskColor(maskColor_)
{ }

LevelState LevelRecord::state() const {
    return solved
        ? LevelState::solved
        : !parent
        ? LevelState::open
        : parent->solved
        ? LevelState::open
        : LevelState::locked;
}

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
    [[maybe_unused]] auto [parent, codename, filename, x, y, color, poster] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string,
            lua::types::integer,
            lua::types::integer,
            lua::optional<lua::types::integer, -1>,
            lua::optional<lua::types::string>
    >(L);
    auto& self = dynamic_cast<Levels&>(Script::from(L).ref());
    std::shared_ptr<LevelRecord> parentRecord = *parent
            ? self.m_levels[parent]
            : decltype(parentRecord){};
    self.m_levels[codename] = std::make_shared<LevelRecord>(parentRecord, filename, false, Coords{x, y}, color);
    return 0;
}

int Levels::branch_setEnding(lua_State* L) {
    [[maybe_unused]] auto [codename, filename, poster] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string
    >(L);
    LOGV("branchEnding");
    return 0;
}

int Levels::worldmap_addDesc(lua_State* L) {
    [[maybe_unused]] auto [codename, lang, levelname, branch] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string,
            lua::types::string
    >(L);
    LOGV("addDesc");
    return 0;
}

int Levels::node_bestSolution(lua_State* L) {
    [[maybe_unused]] auto [codename, moves, name] = lua::args<
            lua::types::string,
            lua::types::integer,
            lua::types::string
    >(L);
    LOGV("bestSolution");
    return 0;
}