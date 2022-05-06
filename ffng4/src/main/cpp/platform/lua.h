#ifndef FISH_FILLETS_LUA_H
#define FISH_FILLETS_LUA_H

#include "common.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace lua {

    class Env {
        lua_State* m_state;

    public:
        Env();
        ~Env();

        void registerFn(const std::string& name, lua_CFunction function);
        void doString(const std::string& string);
    };

}

#endif //FISH_FILLETS_LUA_H