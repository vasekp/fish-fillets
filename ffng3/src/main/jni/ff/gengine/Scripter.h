#ifndef HEADER_SCRIPTER_H
#define HEADER_SCRIPTER_H

class File;
class ScriptState;

#include "NoCopy.h"

#include <string>

/**
 * Base class using a script.
 */
class Scripter : public NoCopy {
    protected:
        ScriptState *m_script;
    public:
        Scripter();
        virtual ~Scripter();

        void scriptInclude(const File &filename);
        void scriptDo(const std::string &input);
};

#endif
