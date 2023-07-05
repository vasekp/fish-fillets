#include "subsystem/script.h"

LuaCallback::LuaCallback(LuaCallback &&other) noexcept :
    m_state(other.m_state),
    m_ref(other.m_ref),
    m_tries(other.m_tries)
{
    other.m_state = nullptr;
}

LuaCallback& LuaCallback::operator=(LuaCallback &&other) noexcept {
    std::swap(m_state, other.m_state);
    std::swap(m_ref, other.m_ref);
    std::swap(m_tries, other.m_tries);
    return *this;
}

LuaCallback::~LuaCallback() {
    if(m_state)
        luaL_unref(m_state, LUA_REGISTRYINDEX, m_ref);
}

LuaCallback LuaCallback::from(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return {L, ref};
}

bool LuaCallback::operator()() {
    Log::verbose<Log::lua>("calling [", m_ref, "] tries=", m_tries);
    lua_rawgeti(m_state, LUA_REGISTRYINDEX, m_ref);
    lua_pushnumber(m_state, m_tries++);
    if(lua_pcall(m_state, 1, 1, 0) != LUA_OK)
        Log::fatal("Lua error: ", lua_tostring(m_state, -1));
    bool result = lua_toboolean(m_state, -1);
    lua_pop(m_state, 1);
    return result;
}

bool Callback::operator()() {
    if(index() == 0)
        return std::get<0>(*this)();
    else
        return std::get<1>(*this)();
}
