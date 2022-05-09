#include "level.h"
#include "levelscreen.h"

Level::Level(Instance& instance, LevelScreen& screen, const LevelRecord& record) :
    m_instance(instance),
    m_screen(screen),
    m_record(record),
    m_script(instance, *this),
    m_models()
{
    m_script.registerFn("game_setRoomWaves", lua::wrap<&Level::game_setRoomWaves>);
    m_script.registerFn("game_addModel", lua::wrap<&Level::game_addModel>);
    m_script.registerFn("game_getCycles", lua::wrap<&Level::game_getCycles>);
//    m_script.registerFn("game_addDecor", script_game_addDecor);
//    m_script.registerFn("game_setScreenShift", script_game_setScreenShift);
//    m_script.registerFn("game_changeBg", script_game_changeBg);
//    m_script.registerFn("game_getBg", script_game_getBg);
//    m_script.registerFn("game_checkActive", script_game_checkActive);
//    m_script.registerFn("game_setFastFalling", script_game_setFastFalling);

    m_script.registerFn("model_addAnim", lua::wrap<&Level::model_addAnim>);
    m_script.registerFn("model_runAnim", lua::wrap<&Level::model_runAnim>);
    m_script.registerFn("model_setAnim", lua::wrap<&Level::model_setAnim>);
    m_script.registerFn("model_useSpecialAnim", lua::wrap<&Level::model_useSpecialAnim>);
//    m_script.registerFn("model_setEffect", script_model_setEffect);
    m_script.registerFn("model_getLoc", lua::wrap<&Level::model_getLoc>);
    m_script.registerFn("model_getAction", lua::wrap<&Level::model_getAction>);
    m_script.registerFn("model_getState", lua::wrap<&Level::model_getState>);
//    m_script.registerFn("model_getTouchDir", script_model_getTouchDir);
    m_script.registerFn("model_isAlive", lua::wrap<&Level::model_isAlive>);
    m_script.registerFn("model_isOut", lua::wrap<&Level::model_isOut>);
    m_script.registerFn("model_isLeft", lua::wrap<&Level::model_isLeft>);
//    m_script.registerFn("model_isAtBorder", script_model_isAtBorder);
//    m_script.registerFn("model_getW", script_model_getW);
//    m_script.registerFn("model_getH", script_model_getH);
    m_script.registerFn("model_setGoal", lua::wrap<&Level::model_setGoal>);
    m_script.registerFn("model_change_turnSide", lua::wrap<&Level::model_change_turnSide>);
//    m_script.registerFn("model_change_setLocation", script_model_change_setLocation);
//    m_script.registerFn("model_setViewShift", script_model_setViewShift);
//    m_script.registerFn("model_getViewShift", script_model_getViewShift);
//    m_script.registerFn("model_setBusy", script_model_setBusy);
//    m_script.registerFn("model_getExtraParams", script_model_getExtraParams);
//    m_script.registerFn("model_change_setExtraParams", script_model_change_setExtraParams);
//    m_script.registerFn("model_equals", script_model_equals);
    m_script.registerFn("model_isTalking", lua::wrap<&Level::model_isTalking>);
//    m_script.registerFn("model_talk", script_model_talk);
//    m_script.registerFn("model_killSound", script_model_killSound);

    m_script.registerFn("sound_addSound", lua::wrap<&Level::sound_addSound>);
//    m_script.registerFn("sound_playSound", script_sound_playSound);
    m_script.registerFn("sound_playMusic", lua::wrap<&Level::sound_playMusic>);
//    m_script.registerFn("sound_stopMusic", script_sound_stopMusic);

    m_script.registerFn("level_createRoom", lua::wrap<&Level::level_createRoom>);
    m_script.registerFn("level_getRestartCounter", lua::wrap<&Level::level_getRestartCounter>);
    m_script.registerFn("level_getDepth", lua::wrap<&Level::level_getDepth>);
    m_script.registerFn("level_isNewRound", lua::wrap<&Level::level_isNewRound>);
    m_script.registerFn("level_isSolved", lua::wrap<&Level::level_isSolved>);
//    m_script.registerFn("level_newDemo", script_level_newDemo);
//    m_script.registerFn("level_planShow", script_level_planShow);
//    m_script.registerFn("level_isShowing", script_level_isShowing);
//    m_script.registerFn("level_save", script_level_save);
//    m_script.registerFn("level_load", script_level_load);
//    m_script.registerFn("level_action_move", script_level_action_move);
//    m_script.registerFn("level_action_save", script_level_action_save);
//    m_script.registerFn("level_action_load", script_level_action_load);
//    m_script.registerFn("level_action_restart", script_level_action_restart);

    m_script.registerFn("game_planAction", lua::wrap<&Level::game_planAction>);
    m_script.registerFn("game_isPlanning", lua::wrap<&Level::game_isPlanning>);
//    m_script.registerFn("game_killPlan", script_game_killPlan);

    m_script.registerFn("dialog_isDialog", lua::wrap<&Level::dialog_isDialog>);
    m_script.registerFn("dialog_addFont", lua::wrap<&Level::dialog_addFont>);
    m_script.registerFn("dialog_addDialog", lua::wrap<&Level::dialog_addDialog>);

    m_script.registerFn("options_getParam", lua::wrap<&Level::options_getParam>);
}

void Level::init() {
    m_script.loadFile(m_record.script_filename);
}

void Level::tick() {
    for(auto& model : models())
        model.anim().update();
    m_script.doString("script_update();");
}

void Level::level_createRoom(int width, int height, const std::string& bg) {
    m_screen.create(width, height, bg);
}

int Level::level_getRestartCounter() {
    //TODO
    return 0;
}

int Level::level_getDepth() const {
    return m_record.depth;
}

bool Level::level_isNewRound() {
    //TODO
    return true;
}

bool Level::level_isSolved() {
    //TODO
    return false;
}

void Level::game_setRoomWaves(float amplitude, float period, float speed) {
    m_screen.setWaves(amplitude, period, speed);
}

int Level::game_addModel(const std::string& type, int x, int y, const std::string& shape) {
    m_models.emplace_back(type, x, y, shape);
    return (int)m_models.size() - 1;
}

int Level::game_getCycles() {
    return m_screen.m_timer.tickCount();
}

void Level::model_addAnim(int index, const std::string& name, const std::string& filename, std::optional<int> direction) {
    auto& model = m_models[index];
    auto image = m_screen.addImage(filename);
    model.anim().add(name, direction.value_or(Model::dir::left), image);
}

void Level::model_runAnim(int index, const std::string& name, std::optional<int> phase) {
    auto& model = m_models[index];
    model.anim().set(name, phase.value_or(0), true);
}

void Level::model_setAnim(int index, const std::string& name, int phase) {
    auto& model = m_models[index];
    model.anim().set(name, phase, false);
}

void Level::model_useSpecialAnim(int index, const std::string& name, int phase) {
    auto& model = m_models[index];
    model.anim().setExtra(name, phase);
}

std::pair<int, int> Level::model_getLoc(int index) {
    auto& model = m_models[index];
    return {model.x(), model.y()};
}

std::string Level::model_getAction(int index) {
//     TODO
    return "rest";
}

std::string Level::model_getState(int index) {
//     TODO
    return "talking";
}

bool Level::model_isAlive(int index) {
    return m_models[index].isAlive();
}

bool Level::model_isOut(int index) {
    // TODO
    return false;
}

bool Level::model_isLeft(int index) {
    auto& model = m_models[index];
    return model.direction() == Model::dir::left;
}

void Level::model_setGoal(int index, const std::string& goal) {
    //TODO
}

void Level::model_change_turnSide(int index) {
    //TODO
}

bool Level::model_isTalking(int index) {
    // TODO
    return false;
}

void Level::sound_addSound(const std::string& name, const std::string& filename) {
    m_screen.addSound(name, filename);
}

void Level::sound_playMusic(const std::string& filename) {
    m_screen.playMusic(filename);
}

bool Level::game_isPlanning() {
    // TODO
    return false;
}

void Level::game_planAction() {
    // TODO
}

bool Level::dialog_isDialog() {
    // TODO
    return false;
}

void Level::dialog_addFont(const std::string& name, int r, int g, int b) {
    // TODO
}

void Level::dialog_addDialog(const std::string& name, const std::string& lang, const std::string& soundfile,
                             const std::optional<std::string>& fontname, const std::optional<std::string>& subtitle) {
    // TODO
}

std::string Level::options_getParam(const std::string& name) {
    // TODO
    return "cs";
}