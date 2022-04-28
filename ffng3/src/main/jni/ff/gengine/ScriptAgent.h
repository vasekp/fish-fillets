#ifndef HEADER_SCRIPTAGENT_H
#define HEADER_SCRIPTAGENT_H

class File;
class ScriptState;

#include "common.h"
#include "BaseAgent.h"
#include "Name.h"
#include "Scripter.h"

extern "C" {
#include "lua.h"
}

/**
 * Global scripting.
 * It is used by OptionAgent.
 */
class ScriptAgent : public BaseAgent, public Scripter {
    AGENT(ScriptAgent, Name::SCRIPT_NAME);
    protected:
        virtual void own_init();
    public:
        void registerFunc(const char *name, lua_CFunction func);
};

#endif
