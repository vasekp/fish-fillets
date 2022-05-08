#ifndef FISH_FILLETS_SCRIPT_H
#define FISH_FILLETS_SCRIPT_H

#include "instance.h"
#include "platform/lua.h"

class ScriptReferrer {
public:
    virtual ~ScriptReferrer() { }
};

class Script {
    Instance& m_instance;
    ScriptReferrer& m_ref;
    lua::Env m_env;

public:
    Script(Instance& instance, ScriptReferrer& ref);
    static Script& from(lua_State*);

    ScriptReferrer& ref() const { return m_ref; }

    void registerFn(const std::string& name, lua_CFunction function);
    void doString(const std::string& string);
    void loadFile(const std::string& filename);

    static void file_include(lua_State* L, const std::string& filename);
    static bool file_exists(lua_State* L, const std::string& filename);
    static void sendMsg(lua_State* L, const std::string& target, const std::string& text);
};

#endif //FISH_FILLETS_SCRIPT_H