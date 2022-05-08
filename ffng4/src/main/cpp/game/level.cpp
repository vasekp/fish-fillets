#include "level.h"
#include "levelscreen.h"

Level::Level(Instance& instance, LevelScreen& screen, std::string filename) :
    m_instance(instance),
    m_screen(screen),
    m_filename(std::move(filename)),
    m_script(instance, *this),
    m_models()
{
    m_script.registerFn("game_setRoomWaves", lua::wrap<game_setRoomWaves>);
    m_script.registerFn("game_addModel", lua::wrap<game_addModel>);
    m_script.registerFn("game_getCycles", lua::wrap<game_getCycles>);
//    m_script.registerFn("game_addDecor", script_game_addDecor);
//    m_script.registerFn("game_setScreenShift", script_game_setScreenShift);
//    m_script.registerFn("game_changeBg", script_game_changeBg);
//    m_script.registerFn("game_getBg", script_game_getBg);
//    m_script.registerFn("game_checkActive", script_game_checkActive);
//    m_script.registerFn("game_setFastFalling", script_game_setFastFalling);
//
    m_script.registerFn("model_addAnim", lua::wrap<model_addAnim>);
    m_script.registerFn("model_runAnim", lua::wrap<model_runAnim>);
    m_script.registerFn("model_setAnim", lua::wrap<model_setAnim>);
//    m_script.registerFn("model_useSpecialAnim", script_model_useSpecialAnim);
//    m_script.registerFn("model_countAnims", script_model_countAnims);
//    m_script.registerFn("model_setEffect", script_model_setEffect);
    m_script.registerFn("model_getLoc", lua::wrap<model_getLoc>);
//    m_script.registerFn("model_getAction", script_model_getAction);
//    m_script.registerFn("model_getState", script_model_getState);
//    m_script.registerFn("model_getDir", script_model_getDir);
//    m_script.registerFn("model_getTouchDir", script_model_getTouchDir);
//    m_script.registerFn("model_isAlive", script_model_isAlive);
//    m_script.registerFn("model_isOut", script_model_isOut);
    m_script.registerFn("model_isLeft", lua::wrap<model_isLeft>);
//    m_script.registerFn("model_isAtBorder", script_model_isAtBorder);
//    m_script.registerFn("model_getW", script_model_getW);
//    m_script.registerFn("model_getH", script_model_getH);
    m_script.registerFn("model_setGoal", lua::wrap<model_setGoal>);
    m_script.registerFn("model_change_turnSide", lua::wrap<model_change_turnSide>);
//    m_script.registerFn("model_change_setLocation", script_model_change_setLocation);
//    m_script.registerFn("model_setViewShift", script_model_setViewShift);
//    m_script.registerFn("model_getViewShift", script_model_getViewShift);
//    m_script.registerFn("model_setBusy", script_model_setBusy);
//    m_script.registerFn("model_getExtraParams", script_model_getExtraParams);
//    m_script.registerFn("model_change_setExtraParams", script_model_change_setExtraParams);
//    m_script.registerFn("model_equals", script_model_equals);
//
    m_script.registerFn("sound_addSound", lua::wrap<sound_addSound>);
//    m_script.registerFn("sound_playSound", script_sound_playSound);
    m_script.registerFn("sound_playMusic", lua::wrap<sound_playMusic>);
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
    m_script.registerFn("level_createRoom", lua::wrap<level_createRoom>);
    m_script.registerFn("level_getRestartCounter", lua::wrap<level_getRestartCounter>);
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
    m_script.registerFn("dialog_addFont", lua::wrap<dialog_addFont>);
    m_script.registerFn("dialog_addDialog", lua::wrap<dialog_addDialog>);
//    m_script.registerFn("model_isTalking", script_model_isTalking);
//    m_script.registerFn("model_talk", script_model_talk);
//    m_script.registerFn("model_killSound", script_model_killSound);

    m_script.registerFn("options_getParam", lua::wrap<options_getParam>);
}

void Level::init() {
    m_script.loadFile(m_filename);
}

void Level::level_createRoom(lua_State *L, int width, int height, const std::string& bg) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.create(width, height, bg);
}

int Level::level_getRestartCounter(lua_State*) {
    //TODO
    return 0;
}

void Level::game_setRoomWaves(lua_State* L, float amplitude, float period, float speed) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.setWaves(amplitude, period, speed);
}

int Level::game_addModel(lua_State* L, const std::string& type, int x, int y, const std::string& shape) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_models.emplace_back(type, x, y, shape);
    return (int)self.m_models.size() - 1;
}

int Level::game_getCycles(lua_State* L) {
    //TODO
    lua_pushnumber(L, 0);
    return 1;
}

void Level::model_addAnim(lua_State* L, int index, const std::string& name, const std::string& filename,
                          std::optional<int> direction) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    auto image = self.m_screen.addImage(filename);
    model.anim().add(name, direction.value_or(Model::dir::left), image);
}

void Level::model_runAnim(lua_State* L, int index, const std::string& name, std::optional<int> phase) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    model.anim().set(name, phase.value_or(0)); // TODO: run
}

void Level::model_setAnim(lua_State* L, int index, const std::string& name, int phase) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    model.anim().set(name, phase);
}

std::pair<int, int> Level::model_getLoc(lua_State* L, int index) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    return {model.x(), model.y()};
}

bool Level::model_isLeft(lua_State* L, int index) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    auto& model = self.m_models[index];
    return model.direction() == Model::dir::left;
}

void Level::model_setGoal(lua_State* L, int index, const std::string& goal) {
    //TODO
}

void Level::model_change_turnSide(lua_State* L, int index) {
    //TODO
}

void Level::sound_addSound(lua_State* L, const std::string& name, const std::string& filename) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.addSound(name, filename);
}

void Level::sound_playMusic(lua_State* L, const std::string& filename) {
    auto& self = dynamic_cast<Level&>(Script::from(L).ref());
    self.m_screen.playMusic(filename);
}

void Level::dialog_addFont(lua_State* L, const std::string& name, int r, int g, int b) {
    // TODO
}

void Level::dialog_addDialog(lua_State* L, const std::string& name, const std::string& lang, const std::string& soundfile,
                             const std::optional<std::string>& fontname, const std::optional<std::string>& subtitle) {
    // TODO
}

std::string Level::options_getParam(lua_State* L, const std::string& name) {
    // TODO
    return "cs";
}