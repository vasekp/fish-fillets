#ifndef HEADER_GAMEINPUT_H
#define HEADER_GAMEINPUT_H

#include "StateInput.h"

/**
 * Input which enables menu options.
 */
class GameInput : public StateInput {
    protected:
        GameInput(GameState *state) : StateInput(state) {}
        virtual void enableHelp();
        virtual void enableMenu();
};

#endif
