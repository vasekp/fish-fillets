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

    static void level_createRoom(lua_State* L, int width, int height, const std::string& bg);
    static int level_getRestartCounter(lua_State* L);
    static void game_setRoomWaves(lua_State* L, float amplitude, float period, float speed);
    static int game_addModel(lua_State* L, const std::string& type, int x, int y, const std::string& shape);
    static int game_getCycles(lua_State*);
    static void model_addAnim(lua_State* L, int index, const std::string& name, const std::string& filename,
                             lua::optional<int, Model::dir::left> direction);
    static void model_runAnim(lua_State* L, int index, const std::string& name, lua::optional<int, 0> phase);
    static void model_setAnim(lua_State* L, int index, const std::string& name, int phase);
    static std::pair<int, int> model_getLoc(lua_State* L, int index);
    static bool model_isLeft(lua_State* L, int index);
    static void model_setGoal(lua_State*, int index, const std::string& goal);
    static void model_change_turnSide(lua_State*, int index);
    static void sound_addSound(lua_State* L, const std::string& name, const std::string& filename);
    static void sound_playMusic(lua_State* L, const std::string& filename);
    static void dialog_addFont(lua_State* L, const std::string& name, int r, int g, int b);
    static void dialog_addDialog(lua_State* L, const std::string& name, const std::string& lang, const std::string& soundfile,
                                 const lua::optional<std::string>& fontname, const lua::optional<std::string>& subtitle);
    static std::string options_getParam(lua_State* L, const std::string& name);
};

#endif //FISH_FILLETS_LEVEL_H