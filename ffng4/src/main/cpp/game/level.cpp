#include "level.h"
#include "levelscreen.h"

Level::Level(Instance& instance, LevelScreen& screen, const std::string filename) :
    m_instance(instance),
    m_screen(screen),
    m_filename(filename),
    m_script(instance, *this),
    m_models()
{
    m_script.registerFn("game_setRoomWaves", game_setRoomWaves);
    m_script.registerFn("game_addModel", game_addModel);
    m_script.registerFn("game_getCycles", game_getCycles);
//    m_script.registerFn("game_addDecor", script_game_addDecor);
//    m_script.registerFn("game_setScreenShift", script_game_setScreenShift);
//    m_script.registerFn("game_changeBg", script_game_changeBg);
//    m_script.registerFn("game_getBg", script_game_getBg);
//    m_script.registerFn("game_checkActive", script_game_checkActive);
//    m_script.registerFn("game_setFastFalling", script_game_setFastFalling);
//
    m_script.registerFn("model_addAnim", model_addAnim);
    m_script.registerFn("model_runAnim", model_runAnim);
    m_script.registerFn("model_setAnim", model_setAnim);
//    m_script.registerFn("model_useSpecialAnim", script_model_useSpecialAnim);
//    m_script.registerFn("model_countAnims", script_model_countAnims);
//    m_script.registerFn("model_setEffect", script_model_setEffect);
    m_script.registerFn("model_getLoc", model_getLoc);
//    m_script.registerFn("model_getAction", script_model_getAction);
//    m_script.registerFn("model_getState", script_model_getState);
//    m_script.registerFn("model_getDir", script_model_getDir);
//    m_script.registerFn("model_getTouchDir", script_model_getTouchDir);
//    m_script.registerFn("model_isAlive", script_model_isAlive);
//    m_script.registerFn("model_isOut", script_model_isOut);
    m_script.registerFn("model_isLeft", model_isLeft);
//    m_script.registerFn("model_isAtBorder", script_model_isAtBorder);
//    m_script.registerFn("model_getW", script_model_getW);
//    m_script.registerFn("model_getH", script_model_getH);
    m_script.registerFn("model_setGoal", model_setGoal);
    m_script.registerFn("model_change_turnSide", model_change_turnSide);
//    m_script.registerFn("model_change_setLocation", script_model_change_setLocation);
//    m_script.registerFn("model_setViewShift", script_model_setViewShift);
//    m_script.registerFn("model_getViewShift", script_model_getViewShift);
//    m_script.registerFn("model_setBusy", script_model_setBusy);
//    m_script.registerFn("model_getExtraParams", script_model_getExtraParams);
//    m_script.registerFn("model_change_setExtraParams", script_model_change_setExtraParams);
//    m_script.registerFn("model_equals", script_model_equals);
//
    m_script.registerFn("sound_addSound", sound_addSound);
//    m_script.registerFn("sound_playSound", script_sound_playSound);
    m_script.registerFn("sound_playMusic", sound_playMusic);
//    m_script.registerFn("sound_stopMusic", script_sound_stopMusic);
//
//    m_script.registerFn("level_save", script_level_save);
//    m_script.registerFn("level_load", script_level_load);
//
//    m_script.registerFn("level_action_move", script_level_action_move);
//    m_script.registerFn("level_action_save", script_level_action_save);
//    m_script.registerFn("level_action_load", script_level_action_load);
//    m_script.registerFn("level_action_restart", script_level_action_restart);
//
    m_script.registerFn("level_createRoom", level_createRoom);
    m_script.registerFn("level_getRestartCounter", level_getRestartCounter);
//    m_script.registerFn("level_getDepth", script_level_getDepth);
//    m_script.registerFn("level_isNewRound", script_level_isNewRound);
//    m_script.registerFn("level_isSolved", script_level_isSolved);
//    m_script.registerFn("level_newDemo", script_level_newDemo);
//    m_script.registerFn("level_planShow", script_level_planShow);
//    m_script.registerFn("level_isShowing", script_level_isShowing);
//
//    m_script.registerFn("game_planAction", script_game_planAction);
//    m_script.registerFn("game_isPlanning", script_game_isPlanning);
//    m_script.registerFn("game_killPlan", script_game_killPlan);
//
//    m_script.registerFn("dialog_isDialog", script_dialog_isDialog);
    m_script.registerFn("dialog_addFont", dialog_addFont);
    m_script.registerFn("dialog_addDialog", dialog_addDialog);
//    m_script.registerFn("model_isTalking", script_model_isTalking);
//    m_script.registerFn("model_talk", script_model_talk);
//    m_script.registerFn("model_killSound", script_model_killSound);

    m_script.registerFn("options_getParam", options_getParam);
}

void Level::init() {
    m_script.loadFile(m_filename);
}

int Level::level_createRoom(lua_State *L) {
    auto [width, height, bg] = lua::args<lua::types::integer, lua::types::integer, lua::types::string>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.create(width, height, bg);
    return 0;
}

int Level::level_getRestartCounter(lua_State* L) {
    //TODO
    lua_pushnumber(L, 0);
    return 1;
}

int Level::game_setRoomWaves(lua_State* L) {
    auto [amplitude, period, speed] = lua::args<lua::types::number, lua::types::number, lua::types::number>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.setWaves(amplitude, period, speed);
    return 0;
}

int Level::game_addModel(lua_State* L) {
    auto [type, x, y, shape] = lua::args<lua::types::string, lua::types::integer, lua::types::integer, lua::types::string>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_models.emplace_back(type, x, y, shape);
    lua_pushinteger(L, (lua_Integer)self.m_models.size() - 1);
    return 1;
}

int Level::game_getCycles(lua_State* L) {
    //TODO
    lua_pushnumber(L, 0);
    return 1;
}

int Level::model_addAnim(lua_State* L) {
    auto [index, name, filename, direction] = lua::args<lua::types::integer,
        lua::types::string, lua::types::string,
        lua::optional<lua::types::integer, Model::dir::left>>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    auto image = self.m_screen.addImage(filename);
    model.anim().add(name, direction, image);
    return 0;
}

int Level::model_runAnim(lua_State* L) {
    auto [index, name, phase] = lua::args<lua::types::integer,
            lua::types::string, lua::optional<lua::types::integer, 0>>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    model.anim().set(name, phase); // TODO: run
    return 0;
}

int Level::model_setAnim(lua_State* L) {
    auto [index, name, phase] = lua::args<lua::types::integer,
            lua::types::string, lua::types::integer>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    model.anim().set(name, phase);
    return 0;
}

int Level::model_getLoc(lua_State* L) {
    auto [index] = lua::args<lua::types::integer>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    lua_pushinteger(L, model.x());
    lua_pushinteger(L, model.y());
    return 2;
}

int Level::model_isLeft(lua_State* L) {
    auto [index] = lua::args<lua::types::integer>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    lua_pushboolean(L, model.direction() == Model::dir::left);
    return 1;
}

int Level::model_setGoal(lua_State* L) {
    [[maybe_unused]] auto [index, goal] = lua::args<lua::types::integer, lua::types::string>(L);
    //TODO
    return 0;
}

int Level::model_change_turnSide(lua_State* L) {
    [[maybe_unused]] auto [index] = lua::args<lua::types::integer>(L);
    //TODO
    return 0;
}

int Level::sound_addSound(lua_State* L) {
    auto [name, filename] = lua::args<lua::types::string, lua::types::string>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.addSound(name, filename);
    return 0;
}

int Level::sound_playMusic(lua_State* L) {
    auto [filename] = lua::args<lua::types::string>(L);
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.playMusic(filename);
    return 0;
}

int Level::dialog_addFont(lua_State* L) {
    [[maybe_unused]] auto [name, r, g, b] = lua::args<lua::types::string,
        lua::types::integer, lua::types::integer, lua::types::integer>(L);
    // TODO
    return 0;
}

int Level::dialog_addDialog(lua_State* L) {
    [[maybe_unused]] auto [name, lang, soundfile, fontname, subtitle] = lua::args<
            lua::types::string, lua::types::string, lua::types::string,
            lua::optional<lua::types::string>,lua::optional<lua::types::string>>(L);

    // TODO
    return 0;
}

int Level::options_getParam(lua_State* L) {
    [[maybe_unused]] auto [name] = lua::args<lua::types::string>(L);
    // TODO
    lua_pushlstring(L, "cs", 2);
    return 1;
}