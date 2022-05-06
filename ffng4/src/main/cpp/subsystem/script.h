#ifndef FISH_FILLETS_SCRIPT_H
#define FISH_FILLETS_SCRIPT_H

#include "instance.h"
#include "platform/lua.h"

class Script {
    Instance& m_instance;
    GameScreen& m_screen;
    lua::Env m_env;

public:
    Script(Instance& instance, GameScreen& screen);
    static Script& from(lua_State*);

    GameScreen& screen() const { return m_screen; }

    void registerFn(const std::string& name, lua_CFunction function);
    void doString(const std::string& string);
    void loadFile(const std::string& filename);
};

#endif //FISH_FILLETS_SCRIPT_H