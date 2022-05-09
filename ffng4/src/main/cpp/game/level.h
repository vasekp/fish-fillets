#ifndef FISH_FILLETS_LEVEL_H
#define FISH_FILLETS_LEVEL_H

#include "subsystem/script.h"
#include "levelrecord.h"
#include "model.h"

class LevelScreen;

class Level : public ScriptReferrer {
    [[maybe_unused]] Instance& m_instance;
    LevelScreen& m_screen;
    const LevelRecord& m_record;
    Script m_script;
    std::vector<Model> m_models;

public:
    Level(Instance& instance, LevelScreen& screen, const LevelRecord& record);

    std::vector<Model>& models() { return m_models; }

    void init();
    void tick();

    static void level_createRoom(lua_State* L, int width, int height, const std::string& bg);
    static int level_getRestartCounter(lua_State* L);
    static int level_getDepth(lua_State* L);
    static bool level_isNewRound(lua_State* L);
    static bool level_isSolved(lua_State* L);
    static void game_setRoomWaves(lua_State* L, float amplitude, float period, float speed);
    static int game_addModel(lua_State* L, const std::string& type, int x, int y, const std::string& shape);
    static int game_getCycles(lua_State*);
    static void model_addAnim(lua_State* L, int index, const std::string& name, const std::string& filename,
                             std::optional<int> direction);
    static void model_runAnim(lua_State* L, int index, const std::string& name, std::optional<int> phase);
    static void model_setAnim(lua_State* L, int index, const std::string& name, int phase);
    static void model_useSpecialAnim(lua_State* L, int index, const std::string& name, int phase);
    static std::pair<int, int> model_getLoc(lua_State* L, int index);
    static std::string model_getAction(lua_State* L, int index);
    static std::string model_getState(lua_State* L, int index);
    static bool model_isAlive(lua_State* L, int index);
    static bool model_isOut(lua_State* L, int index);
    static bool model_isLeft(lua_State* L, int index);
    static void model_setGoal(lua_State*, int index, const std::string& goal);
    static void model_change_turnSide(lua_State*, int index);
    static bool model_isTalking(lua_State* L, int index);
    static void sound_addSound(lua_State* L, const std::string& name, const std::string& filename);
    static void sound_playMusic(lua_State* L, const std::string& filename);
    static bool game_isPlanning(lua_State* L);
    static void game_planAction(lua_State* L/*, TODO */);
    static bool dialog_isDialog(lua_State* L);
    static void dialog_addFont(lua_State* L, const std::string& name, int r, int g, int b);
    static void dialog_addDialog(lua_State* L, const std::string& name, const std::string& lang, const std::string& soundfile,
                                 const std::optional<std::string>& fontname, const std::optional<std::string>& subtitle);
    static std::string options_getParam(lua_State* L, const std::string& name);
};

#endif //FISH_FILLETS_LEVEL_H