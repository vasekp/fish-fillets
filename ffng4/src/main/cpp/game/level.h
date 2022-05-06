#ifndef FISH_FILLETS_LEVEL_H
#define FISH_FILLETS_LEVEL_H

#include "subsystem/script.h"
#include "model.h"

class LevelScreen;

class Level : public ScriptReferrer {
    [[maybe_unused]] Instance& m_instance;
    LevelScreen& m_screen;
    std::string m_filename;
    Script m_script;
    std::vector<Model> m_models;

public:
    Level(Instance&, LevelScreen&, std::string);

    const std::vector<Model>& models() { return m_models; }

    void init();

    static int level_createRoom(lua_State*);
    static int level_getRestartCounter(lua_State*);
    static int game_setRoomWaves(lua_State*);
    static int game_addModel(lua_State*);
    static int game_getCycles(lua_State*);
    static int model_addAnim(lua_State*);
    static int model_runAnim(lua_State*);
    static int model_setAnim(lua_State*);
    static int model_getLoc(lua_State*);
    static int model_isLeft(lua_State*);
    static int model_setGoal(lua_State*);
    static int model_change_turnSide(lua_State*);
    static int sound_addSound(lua_State*);
    static int sound_playMusic(lua_State*);
    static int dialog_addFont(lua_State*);
    static int dialog_addDialog(lua_State*);
    static int options_getParam(lua_State*);
};

#endif //FISH_FILLETS_LEVEL_H