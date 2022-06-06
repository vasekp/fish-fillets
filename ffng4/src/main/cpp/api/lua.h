#ifndef FISH_FILLETS_LUA_H
#define FISH_FILLETS_LUA_H

#include "common.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

#include "./lua/wrapper.h"

namespace lua {

    class Env {
        lua_State* m_state;

    public:
        Env() : m_state(luaL_newstate()) { luaL_openlibs(m_state); }
        ~Env() { lua_close(m_state); }

        operator lua_State*() { return m_state; }
    };

}

#endif //FISH_FILLETS_LUA_H
