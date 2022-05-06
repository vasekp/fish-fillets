#include "subsystem/script.h"
#include "subsystem/files.h"

Script::Script(Instance &instance, ScriptReferrer& ref) :
        m_instance(instance), m_ref(ref), m_env()
{
    lua_pushstring(m_env, "leader");
    lua_pushlightuserdata(m_env, this);
    lua_rawset(m_env, LUA_REGISTRYINDEX);

    registerFn("file_include", file_include);
}

Script& Script::from(lua_State* L) {
    lua_pushstring(L, "leader");
    lua_rawget(L, LUA_REGISTRYINDEX);
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    auto* env = static_cast<Script*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return *env;
}

void Script::registerFn(const std::string& name, lua_CFunction function) {
    lua_register(m_env, name.c_str(), function);
}

void Script::doString(const std::string& string) {
    if(luaL_dostring(m_env, string.c_str()) != LUA_OK) {
        LOGE("Lua error: %s", lua_tostring(m_env, -1));
    }
}

void Script::loadFile(const std::string& filename) {
    doString(m_instance.files().system(filename).read());
}

int Script::file_include(lua_State* L) {
    auto [filename] = lua::args<lua::types::string>(L);
    Script::from(L).loadFile(filename);
    return 0;
}