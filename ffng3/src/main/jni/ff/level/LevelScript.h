#ifndef HEADER_LEVELSCRIPT_H
#define HEADER_LEVELSCRIPT_H

class V2;
class File;
class Cube;
class Unit;
class Level;
class Command;

#include "Planner.h"
#include "RoomAccess.h"

#include <string>
#include <memory>

/**
 * Handle plan for dialogs and planned actions.
 */
class LevelScript : public Planner, public RoomAccess {
    private:
        Level *m_level;
    private:
        void registerGameFuncs();
        void registerLevelFuncs();
    public:
        LevelScript(Level *aLevel);

        void updateScript();
        virtual void interruptPlan();

        Level *level() { return m_level; }
        Command *createCommand(int funcRef);

        int addModel(std::unique_ptr<Cube> model, std::unique_ptr<Unit> unit);
        Cube *getModel(int model_index);
        Cube *askField(const V2 &loc);

        void addSound(const std::string &name, const File &file);
        void playSound(const std::string &name, int volume);
};

#endif
