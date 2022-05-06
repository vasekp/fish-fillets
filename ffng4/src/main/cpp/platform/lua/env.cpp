#include "platform/lua.h"

namespace lua {

    Env::Env() : m_state(luaL_newstate()) {}

    Env::~Env() {
        lua_close(m_state);
    }

    void Env::registerFn(const std::string &name, lua_CFunction function) {
        lua_register(m_state, name.c_str(), function);
    }

    void Env::doString(const std::string &string) {
        if(luaL_dostring(m_state, string.c_str()) != LUA_OK) {
            LOGE("Lua error: %s", lua_tostring(m_state, -1));
        }
    }

}