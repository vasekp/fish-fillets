#include "level.h"
#include "game/screens/levelscreen.h"
#include "subsystem/rng.h"

Level::Level(Instance& instance, LevelScreen& screen, const LevelRecord& record) :
        m_instance(instance),
        m_screen(screen),
        m_record(record),
        m_script(instance, *this),
        m_blockCountdown(0)
{
    m_script.registerFn("game_setRoomWaves", lua::wrap<&Level::game_setRoomWaves>);
    m_script.registerFn("game_addModel", lua::wrap<&Level::game_addModel>);
    m_script.registerFn("game_getCycles", lua::wrap<&Level::game_getCycles>);
//    m_script.registerFn("game_addDecor", script_game_addDecor);
//    m_script.registerFn("game_setScreenShift", script_game_setScreenShift);
//    m_script.registerFn("game_changeBg", script_game_changeBg);
//    m_script.registerFn("game_checkActive", script_game_checkActive);
//    m_script.registerFn("game_setFastFalling", script_game_setFastFalling);
//    m_script.registerFn("game_getBg", script_game_getBg); // UNDO

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
    m_script.registerFn("model_getW", lua::wrap<&Level::model_getW>);
    m_script.registerFn("model_getH", lua::wrap<&Level::model_getH>);
    m_script.registerFn("model_setGoal", lua::wrap<&Level::model_setGoal>);
    m_script.registerFn("model_change_turnSide", lua::wrap<&Level::model_change_turnSide>);
//    m_script.registerFn("model_setViewShift", script_model_setViewShift);
//    m_script.registerFn("model_getViewShift", script_model_getViewShift);
    m_script.registerFn("model_setBusy", lua::wrap<&Level::model_setBusy>);
    m_script.registerFn("model_isTalking", lua::wrap<&Level::model_isTalking>);
    m_script.registerFn("model_talk", lua::wrap<&Level::model_talk>);
    m_script.registerFn("model_killSound", lua::wrap<&Level::model_killSound>);
//    m_script.registerFn("model_equals", script_model_equals);
//    m_script.registerFn("model_change_setLocation", script_model_change_setLocation); // UNDO
//    m_script.registerFn("model_getExtraParams", script_model_getExtraParams); // UNDO
//    m_script.registerFn("model_change_setExtraParams", script_model_change_setExtraParams); // UNDO

    m_script.registerFn("sound_addSound", lua::wrap<&Level::sound_addSound>);
    m_script.registerFn("sound_playSound", lua::wrap<&Level::sound_playSound>);
    m_script.registerFn("sound_playMusic", lua::wrap<&Level::sound_playMusic>);
    m_script.registerFn("sound_stopMusic", lua::wrap<&Level::sound_stopMusic>);

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
    m_script.registerFn("game_killPlan", lua::wrap<&Level::game_killPlan>);

    m_script.registerFn("dialog_isDialog", lua::wrap<&Level::dialog_isDialog>);
    m_script.registerFn("dialog_addFont", lua::wrap<&Level::dialog_addFont>);
    m_script.registerFn("dialog_addDialog", lua::wrap<&Level::dialog_addDialog>);

    m_script.registerFn("options_getParam", lua::wrap<&Level::options_getParam>);
}

void Level::init() {
    m_script.loadFile(m_record.script_filename);
    layout().prepare();
}

void Level::tick() {
    for (auto& model : layout().models())
        model->anim().update();
    if(blocked()) {
        if(--m_blockCountdown == 0)
            m_blockCallback();
    }
    m_script.doString("script_update();");
    if (!m_plan.empty()) {
        auto &front = m_plan.front();
        if(front.call())
            m_plan.pop_front();
    }
}

void Level::blockFor(int frames, const std::function<void()>& callback) {
    m_blockCountdown = frames;
    m_blockCallback = callback;
}

bool Level::blocked() {
    return m_blockCountdown > 0;
}

LevelLayout& Level::layout() {
    return *m_layout;
}

void Level::level_createRoom(int width, int height, const std::string& bg) {
    m_screen.create(width, height, bg);
    m_layout = std::make_unique<LevelLayout>(*this, width, height);
}

int Level::level_getRestartCounter() {
    //TODO
    return 1;
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
    return layout().addModel(type, x, y, shape);
}

int Level::game_getCycles() {
    return m_screen.m_timer.tickCount();
}

void Level::model_addAnim(int index, const std::string& name, const std::string& filename, std::optional<int> orientation) {
    auto image = m_screen.addImage(filename);
    layout().getModel(index).anim().add(name, orientation.value_or(Model::Orientation::left), image);
}

void Level::model_runAnim(int index, const std::string& name, std::optional<int> phase) {
    auto& model = layout().getModel(index);
    model.anim().set(name, model.orientation(), phase.value_or(0), true);
}

void Level::model_setAnim(int index, const std::string& name, int phase) {
    auto& model = layout().getModel(index);
    model.anim().set(name, model.orientation(), phase, false);
}

void Level::model_useSpecialAnim(int index, const std::string& name, int phase) {
    auto& model = layout().getModel(index);
    model.anim().setExtra(name, model.orientation(), phase);
}

std::pair<int, int> Level::model_getLoc(int index) {
    auto& model = layout().getModel(index);
    return {model.x(), model.y()};
}

std::string Level::model_getAction(int index) {
    const auto& model = layout().getModel(index);
    switch(model.action()) {
        case Model::Action::busy:
            return "busy";
        case Model::Action::turning:
            return "turn";
        case Model::Action::activate:
            return "activate";
        case Model::Action::base:
            break;
    }
    auto dir = model.movingDir();
    if (dir == Direction::up)
        return "move_up";
    else if (dir == Direction::down)
        return "move_down";
    else if (dir == Direction::left)
        return "move_left";
    else if (dir == Direction::right)
        return "move_right";
    else
        return "rest";
}

std::string Level::model_getState(int index) {
//     TODO goout
    const auto& model = layout().getModel(index);
    if(model.pushing())
        return "pushing";
    else if(model.talking())
        return "talking";
    else
        return "normal";
}

bool Level::model_isAlive(int index) {
    return layout().getModel(index).alive();
}

bool Level::model_isOut(int index) {
    // TODO
    return false;
}

bool Level::model_isLeft(int index) {
    auto& model = layout().getModel(index);
    return model.orientation() == Model::Orientation::left;
}

unsigned Level::model_getW(int index) {
    return layout().getModel(index).shape().width();
}

unsigned Level::model_getH(int index) {
    return layout().getModel(index).shape().height();
}

void Level::model_setGoal(int index, const std::string& goal) {
    //TODO
}

void Level::model_change_turnSide(int index) {
    layout().getModel(index).turn();
}

void Level::model_setBusy(int index, bool busy) {
    layout().getModel(index).action() = Model::Action::busy;
}

bool Level::model_isTalking(int index) {
    return layout().getModel(index).talking();
}

void Level::model_talk(int index, const std::string& name, std::optional<int> volume, std::optional<int> loops, bool dialogFlag) {
    auto& dialog = m_dialogs.at(name);
    auto& source = m_screen.addSound(name, dialog.soundFile, true);
    source.setVolume((float)volume.value_or(75) / 100.f);
    if(loops.value_or(0) != 0) {
        assert(loops.value() == -1);
        source.setLoop();
    } else
        source.setLoop(0, 0);
    source.setDialog(dialogFlag);
    if(index != -1) { // TODO
        auto &model = layout().getModel(index);
        model.talk() = source;
    }
    m_instance.audio().addSource(source);
    if(!dialog.text.empty())
        m_screen.addSubtitle(dialog.text, dialog.colors);
}

void Level::model_killSound(int index) {
    model_killSoundImpl(layout().getModel(index));
}

void Level::model_killSoundImpl(Model& model) {
    const auto& talk = model.talk();
    if(talk)
        m_instance.audio().removeSource(talk);
    model.talk() = {};
}

void Level::sound_addSound(const std::string& name, const std::string& filename) {
    m_screen.addSound(name, filename);
}

void Level::sound_playSound(const std::string& name, std::optional<int> volume) {
    auto& multimap = m_screen.m_sounds;
    auto size = multimap.count(name);
    auto it = multimap.lower_bound(name);
    std::advance(it, (int)(m_instance.rng().randomIndex(size)));
    auto& source = it->second;
    source.setVolume((float)volume.value_or(100) / 100.f);
    m_instance.audio().addSource(it->second);
}

void Level::sound_playMusic(const std::string& filename) {
    m_screen.playMusic(filename);
}

void Level::sound_stopMusic() {
    m_screen.stopMusic();
}

bool Level::game_isPlanning() {
    return !m_plan.empty();
}

void Level::game_planAction(DelayedFunction function) {
    m_plan.push_back(std::move(function));
}

void Level::game_killPlan() {
    decltype(m_plan) empty{};
    m_plan.swap(empty);
}

bool Level::dialog_isDialog() {
    return m_instance.audio().isDialog();
}

void Level::dialog_addFont(const std::string& name, int r1, int g1, int b1, std::optional<int> r2, std::optional<int> g2, std::optional<int> b2) {
    m_screen.m_subs.defineColors(name, {r1, g1, b1}, {r2.value_or(r1), g2.value_or(g1), b2.value_or(b1)});
}

void Level::dialog_addDialog(const std::string& name, const std::string& lang, const std::string& soundfile,
                             const std::optional<std::string>& fontname, const std::optional<std::string>& subtitle) {
    // TODO
    if(!soundfile.empty() && (lang == "cs" || lang == "en"))
        m_dialogs.insert({name, {subtitle.value_or(""s), fontname.value_or(""), soundfile}});
}

std::string Level::options_getParam(const std::string& name) {
    // TODO
    return "cs";
}