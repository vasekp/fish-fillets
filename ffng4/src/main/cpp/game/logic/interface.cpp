#include "level.h"
#include "game/screens/levelscreen.h"
#include "subsystem/rng.h"

#include <sstream>

void Level::registerCallbacks() {
    m_script.registerFn("game_setRoomWaves", lua::wrap<&Level::game_setRoomWaves>);
    m_script.registerFn("game_addModel", lua::wrap<&Level::game_addModel>);
    m_script.registerFn("game_getCycles", lua::wrap<&Level::game_getCycles>);
    m_script.registerFn("game_addDecor", lua::wrap<&Level::game_addDecor>);
    m_script.registerFn("game_setScreenShift", lua::wrap<&Level::game_setScreenShift>);
    m_script.registerFn("game_changeBg", lua::wrap<&Level::game_changeBg>);
//    m_script.registerFn("game_checkActive", script_game_checkActive);
//    m_script.registerFn("game_setFastFalling", script_game_setFastFalling);
//    m_script.registerFn("game_getBg", script_game_getBg); // UNDO

    m_script.registerFn("model_addAnim", lua::wrap<&Level::model_addAnim>);
    m_script.registerFn("model_runAnim", lua::wrap<&Level::model_runAnim>);
    m_script.registerFn("model_setAnim", lua::wrap<&Level::model_setAnim>);
    m_script.registerFn("model_useSpecialAnim", lua::wrap<&Level::model_useSpecialAnim>);
    m_script.registerFn("model_setEffect", lua::wrap<&Level::model_setEffect>);
    m_script.registerFn("model_getLoc", lua::wrap<&Level::model_getLoc>);
    m_script.registerFn("model_getAction", lua::wrap<&Level::model_getAction>);
    m_script.registerFn("model_getState", lua::wrap<&Level::model_getState>);
    m_script.registerFn("model_getTouchDir", lua::wrap<&Level::model_getTouchDir>);
    m_script.registerFn("model_isAlive", lua::wrap<&Level::model_isAlive>);
    m_script.registerFn("model_isAtBorder", lua::wrap<&Level::model_isAtBorder>);
    m_script.registerFn("model_isOut", lua::wrap<&Level::model_isOut>);
    m_script.registerFn("model_isLeft", lua::wrap<&Level::model_isLeft>);
    m_script.registerFn("model_getW", lua::wrap<&Level::model_getW>);
    m_script.registerFn("model_getH", lua::wrap<&Level::model_getH>);
    m_script.registerFn("model_setGoal", lua::wrap<&Level::model_setGoal>);
    m_script.registerFn("model_change_turnSide", lua::wrap<&Level::model_change_turnSide>);
    m_script.registerFn("model_setViewShift", lua::wrap<&Level::model_setViewShift>);
    m_script.registerFn("model_getViewShift", lua::wrap<&Level::model_getViewShift>);
    m_script.registerFn("model_setBusy", lua::wrap<&Level::model_setBusy>);
    m_script.registerFn("model_isTalking", lua::wrap<&Level::model_isTalking>);
    m_script.registerFn("model_talk", lua::wrap<&Level::model_talk>);
    m_script.registerFn("model_killSound", lua::wrap<&Level::model_killSound>);
    m_script.registerFn("model_equals", lua::wrap<&Level::model_equals>);
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
    m_script.registerFn("level_planShow", lua::wrap<&Level::level_planShow>);
    m_script.registerFn("level_isShowing", lua::wrap<&Level::level_isShowing>);
    m_script.registerFn("level_action_move", lua::wrap<&Level::level_action_move>);
    m_script.registerFn("level_action_save", lua::wrap<&Level::level_action_save>);
    m_script.registerFn("level_action_load", lua::wrap<&Level::level_action_load>);
    m_script.registerFn("level_action_restart", lua::wrap<&Level::level_action_restart>);
    m_script.registerFn("level_save", lua::wrap<&Level::level_save>);
    m_script.registerFn("level_load", lua::wrap<&Level::level_load>);

    m_script.registerFn("level_newDemo", lua::wrap<&Level::level_newDemo>);
    m_script.registerFn("demo_display", lua::wrap<&Level::demo_display>);

    m_script.registerFn("game_planAction", lua::wrap<&Level::game_planAction>);
    m_script.registerFn("game_isPlanning", lua::wrap<&Level::game_isPlanning>);
    m_script.registerFn("game_killPlan", lua::wrap<&Level::game_killPlan>);

    m_script.registerFn("dialog_isDialog", lua::wrap<&Level::dialog_isDialog>);
    m_script.registerFn("dialog_addFont", lua::wrap<&Level::dialog_addFont>);
    m_script.registerFn("dialog_addDialog", lua::wrap<&Level::dialog_addDialog>);

    m_script.registerFn("options_getParam", lua::wrap<&Level::options_getParam>);
}

void Level::level_createRoom(int width, int height, const std::string& bg) {
    m_layout = std::make_unique<LevelLayout>(*this, width, height);
    m_screen.addImage(bg, "background");
}

int Level::level_getRestartCounter() {
    //TODO
    return 1;
}

int Level::level_getDepth() const {
    return m_record.depth;
}

bool Level::level_isNewRound() const {
    return !transitioning();
}

bool Level::level_isSolved() {
    //TODO
    return false;
}

void Level::level_planShow(LuaCallback function) {
    schedule(std::move(function));
}

bool Level::level_isShowing() {
    return isBusy(BusyReason::schedule);
}

bool Level::level_action_move(const std::string& move) {
    assert(move.length() == 1);
    rules().keyInput(Input::CharKeymap(move[0]));
    return true;
}

bool Level::level_action_restart() {
    LOGD("action: restart");
    m_tickSchedule.clear();
    m_tickSchedule.emplace_back([&]() {
        m_dialogs.clear();
        m_screen.restore();
        init();
        m_transitions.clear();
        m_replay.clear();
        setBusy(BusyReason::demo, false);
        setBusy(BusyReason::loading, false);
        // Move schedule can reach over restarts
        return true;
    });
    return true;
}

bool Level::level_action_save() {
    LOGD("action: save");
    if(m_rules->solvable())
        m_script.doString("script_save()");
    input().setLoadPossible(true);
    return true;
}

bool Level::level_action_load() {
    LOGD("action: load");
    if (auto file = saveFile(); file.exists()) {
        m_tickSchedule.clear();
        m_tickSchedule.emplace_back([&, file]() {
            // TODO DRY
            m_dialogs.clear();
            m_screen.restore();
            init();
            m_transitions.clear();
            m_replay.clear();
            setBusy(BusyReason::demo, false);
            setBusy(BusyReason::loading, false);
            // Move schedule can reach over restarts
            m_script.loadFile(file);
            m_script.doString("script_load()");
            return true;
        });
    }
    return true;
}

bool Level::level_save(const std::string& text_models) {
    LOGD("save(text_models)");
    auto file = m_instance.files().user("saves/"s + m_record.codename + ".lua");
    std::ostringstream oss;
    oss << "saved_moves = '" << m_replay << "'\n\nsaved_models = " << text_models;
    file.write(oss.str());
    return true;
}

bool Level::level_load(const std::string& text_moves) {
    LOGD("load(text_moves)");
    setBusy(BusyReason::loading);
    m_layout->speed() = LevelLayout::speed_loading;
    std::vector<Callback> loadMoves;
    for(const auto c : text_moves)
        loadMoves.emplace_back([&,c]() {
            m_rules->keyInput(Input::CharKeymap(c));
            return true;
        });
    loadMoves.emplace_back([&] {
        setBusy(BusyReason::loading, false);
        m_layout->speed() = LevelLayout::speed_normal;
        return true; });
    m_moveSchedule.insert(m_moveSchedule.begin(), std::make_move_iterator(loadMoves.begin()), std::make_move_iterator(loadMoves.end()));
    setBusy(BusyReason::schedule);
    return true;
}

void Level::level_newDemo(const std::string& filename) {
    if(!isBusy(BusyReason::loading)) {
        setBusy(BusyReason::demo);
        m_script.file_include(filename);
    }
}

void Level::demo_display(const std::string& filename) {
    /* NOTE: Lua also passes int x, int y, expecting us to compose the images.
     * These were replaced by pre-composed graphics so everything is drawn at (0,0).
     * The original assets are 10 times smaller, but they would require either storing the intermediate textures
     * (and rewriting all code relying on images loadable from filename) or drawing all previous frames on each draw(). */
    m_screen.display(filename);
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
    auto* image = m_screen.addImage(filename);
    layout().getModel(index)->anim().add(name, orientation.value_or(Model::Orientation::left), image);
}

void Level::model_runAnim(int index, const std::string& name, std::optional<int> phase) {
    layout().getModel(index)->anim().set(name, phase.value_or(0), true);
}

void Level::model_setAnim(int index, const std::string& name, int phase) {
    layout().getModel(index)->anim().set(name, phase, false);
}

void Level::model_useSpecialAnim(int index, const std::string& name, int phase) {
    layout().getModel(index)->anim().setExtra(name, phase);
}

void Level::model_setEffect(int index, const std::string &name) {
    setModelEffect(layout().getModel(index), name);
}

void Level::setModelEffect(Model* model, const std::string& name) {
    m_screen.setEffect(model, name);
}

std::pair<int, int> Level::model_getLoc(int index) {
    return layout().getModel(index)->xy();
}

std::string Level::model_getAction(int index) {
    const auto* model = layout().getModel(index);
    switch(model->action()) {
        case Model::Action::busy:
            return "busy";
        case Model::Action::turning:
            return "turn";
        case Model::Action::activate:
            return "activate";
        case Model::Action::base:
            break;
    }
    auto dir = model->movingDir();
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
    const auto* model = layout().getModel(index);
    if(!model->alive())
        return "dead";
    else if(model->talking())
        return "talking";
    else if(model->pushing())
        return "pushing";
    else if(layout().borderDepth(model).first == 1)
        return "goout";
    else
        return "normal";
}

int Level::model_getTouchDir(int index) {
    auto dir = layout().getModel(index)->touchDir();
    if(dir == Direction::up)
        return 1;
    else if(dir == Direction::down)
        return 2;
    else if(dir == Direction::left)
        return 3;
    else if(dir == Direction::right)
        return 4;
    else
        return 0;
}

bool Level::model_isAlive(int index) {
    return layout().getModel(index)->alive();
}

bool Level::model_isAtBorder(int index) {
    return layout().borderDepth(layout().getModel(index)).first == 0;
}

bool Level::model_isOut(int index) {
    return layout().borderDepth(layout().getModel(index)).second >= 0;
}

bool Level::model_isLeft(int index) {
    return layout().getModel(index)->orientation() == Model::Orientation::left;
}

unsigned Level::model_getW(int index) {
    return layout().getModel(index)->shape().width();
}

unsigned Level::model_getH(int index) {
    return layout().getModel(index)->shape().height();
}

void Level::model_setGoal(int index, const std::string& goal) {
    if(goal == "goal_alive")
        layout().getModel(index)->goal() = Model::Goal::alive;
    else if(goal == "goal_escape" || goal == "goal_out") // FIXME: is there any need to differentiate?
        layout().getModel(index)->goal() = Model::Goal::escape;
}

void Level::model_change_turnSide(int index) {
    layout().getModel(index)->turn();
}

void Level::model_setViewShift(int index, int dx, int dy) {
    layout().getModel(index)->viewShift() = {dx, dy};
}

std::pair<int, int> Level::model_getViewShift(int index) {
    return layout().getModel(index)->viewShift();
}

void Level::model_setBusy(int index, bool busy) {
    layout().getModel(index)->action() = busy ? Model::Action::busy : Model::Action::base;
}

bool Level::model_isTalking(int index) {
    return layout().getModel(index)->talking();
}

void Level::model_talk(int index, const std::string& name, std::optional<int> volume, std::optional<int> loops, bool dialogFlag) {
    auto& dialog = m_dialogs.at(name);
    auto source = m_screen.addSound(name, dialog.soundFile, true);
    source->setVolume((float)volume.value_or(75) / 100.f);
    if(loops.value_or(0) != 0) {
        assert(loops.value() == -1);
        source->setLoop();
    } else
        source->setLoop(0, 0);
    source->setDialog(dialogFlag);
    if(index != index_talk_both)
        layout().getModel(index)->talk() = source;
    else {
        auto [small, big] = rules().bothFish();
        small->talk() = source;
        big->talk() = source;
    }
    m_instance.audio().addSource(source);
    if(!dialog.text.empty())
        m_screen.addSubtitle(dialog.text, dialog.colors);
}

void Level::model_killSound(int index) {
    killModelSound(layout().getModel(index));
}

void Level::killModelSound(Model* model) {
    const auto& talk = model->talk();
    if(talk)
        m_instance.audio().removeSource(talk);
    model->talk() = {};
}

bool Level::model_equals(int index, int x, int y) {
    LOGV("[%d,%d] equals %d?", x, y, index);
    if(x < 0 || x >= layout().width() || y < 0 || y >= layout().height())
        return false;
    if(index != index_free_space) {
        const auto* model = layout().getModel(index);
        return model->shape().covers(ICoords{x, y} - model->xy());
    } else {
        for(const auto& model : layout().models())
            if(model->shape().covers({x - model->x(), y - model->y()})) {
                LOGV("found model %d", model->index());
                return false;
            }
        // none found
        return true;
    }
}

void Level::sound_addSound(const std::string& name, const std::string& filename) {
    m_screen.addSound(name, filename);
}

void Level::sound_playSound(const std::string& name, std::optional<int> volume) {
    playSound(name, (float)volume.value_or(100) / 100.f);
}

void Level::playSound(const std::string& name, float volume) {
    auto& multimap = m_screen.m_sounds;
    auto size = multimap.count(name);
    auto it = multimap.lower_bound(name);
    std::advance(it, (int)(m_instance.rng().randomIndex(size)));
    auto& source = it->second;
    source->setVolume(volume);
    m_instance.audio().addSource(it->second);
}

void Level::sound_playMusic(const std::string& filename) {
    m_screen.playMusic(filename);
}

void Level::sound_stopMusic() {
    m_screen.stopMusic();
}

bool Level::game_isPlanning() {
    return !m_tickSchedule.empty();
}

void Level::game_planAction(LuaCallback function) {
    m_tickSchedule.emplace_back(std::move(function));
}

void Level::game_killPlan() {
    killDialogs();
}

void Level::killDialogs() {
    m_tickSchedule.clear();
}

void Level::killDialogsHard() {
    killDialogs();
    m_instance.audio().clear();
    m_screen.m_subs.clear();
    if(m_screen.m_music)
        m_instance.audio().addSource(m_screen.m_music);
}

void Level::game_addDecor(const std::string& type, int m1, int m2, int dx1, int dy1, int dx2, int dy2) {
    if(type == "rope")
        m_layout->addRope(m_layout->getModel(m1), m_layout->getModel(m2), {dx1, dy1}, {dx2, dy2});
    else
        LOGE("Unknown decor %s", type.c_str());
}

void Level::game_setScreenShift(float dx, float dy) {
    m_screen.setShift({dx, dy});
}

void Level::game_changeBg(const std::string &filename) {
    m_screen.replaceImage("background", filename);
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