#ifndef FISH_FILLETS_SCRIPT_H
#define FISH_FILLETS_SCRIPT_H

#include "instance.h"
#include "platform/lua.h"
#include "script/queuedfn.h"

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
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    static Script& from(lua_State*);

    ScriptReferrer& ref() const { return m_ref; }

    void registerFn(const std::string& name, lua_CFunction function);
    void doString(const std::string& string);
    void loadFile(const std::string& filename);

    void file_include(const std::string& filename);
    bool file_exists(const std::string& filename);
    void sendMsg(const std::string& target, const std::string& text);
};

#include "script/wrapper-additions.h"

#endif //FISH_FILLETS_SCRIPT_H