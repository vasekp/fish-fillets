#ifndef HEADER_SCRIPTSTATE_H
#define HEADER_SCRIPTSTATE_H

class File;
class Scripter;

#include "common.h"
#include "NoCopy.h"

extern "C" {
#include "lua.h"
}

/**
 * Independent script state.
 */
class ScriptState : public NoCopy {
    private:
        lua_State *m_state;
        int m_errorHandlerIndex;
    private:
        void prepareErrorHandler();
        void insertErrorHandler(int index);
        void callStack(int error, int params=0, int returns=0);
    public:
        ScriptState();
        ~ScriptState();

        void doFile(const File &file);
        void doString(const std::string &input);
        bool callCommand(int funcRef, int param);
        void unref(int funcRef);

        void registerFunc(const char *name, lua_CFunction func);
        void registerLeader(Scripter *leader);
};

#endif
