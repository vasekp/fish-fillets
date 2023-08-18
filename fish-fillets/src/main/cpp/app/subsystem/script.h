#ifndef FISH_FILLETS_SCRIPT_H
#define FISH_FILLETS_SCRIPT_H

#include "common.h"
#include "api/lua.h"
#include "script/callback.h"

class Instance;

class ScriptReferrer {
public:
    virtual ~ScriptReferrer() { }
};

class Script {
public:
    Script(Instance& instance, ScriptReferrer& ref);
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    static Script& from(lua_State*);

    ScriptReferrer& ref() const { return m_ref; }

    void registerFn(const std::string& name, lua_CFunction function);
    void doString(const std::string& string);
    void loadFile(const std::string& filename);

    bool file_exists(const std::string& filename);

private:
    Instance& m_instance;
    ScriptReferrer& m_ref;
    lua::Env m_env;
};

#include "script/wrapper-additions.h"

#endif //FISH_FILLETS_SCRIPT_H
