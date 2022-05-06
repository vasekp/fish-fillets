#ifndef FISH_FILLETS_SCRIPT_H
#define FISH_FILLETS_SCRIPT_H

#include "instance.h"
#include "platform/lua.h"

class Script {
    Instance& m_instance;
    lua::Env m_env;

public:
    Script(Instance& instance);
    static Script& from(lua_State*);

    void registerFn(const std::string& name, lua_CFunction function);
    void doString(const std::string& string);
    void loadFile(const std::string& filename);
};

#endif //FISH_FILLETS_SCRIPT_H