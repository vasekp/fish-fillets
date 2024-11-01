#include "level.h"
#include "layout.h"
#include "rules.h"
#include "game/screens/levelscreen.h"
#include "subsystem/files.h"
#include "subsystem/persist.h"
#include "subsystem/audio.h"
#include "subsystem/rng.h"

#include <sstream>

void Level::registerCallbacks() {
    m_script.registerFn("game_setRoomWaves", lua::wrap<&Level::game_setRoomWaves>);
    m_script.registerFn("game_addModel", lua::wrap<&Level::game_addModel>);
    m_script.registerFn("game_getCycles", lua::wrap<&Level::game_getCycles>);
    m_script.registerFn("game_addDecor", lua::wrap<&Level::game_addDecor>);
    m_script.registerFn("game_setScreenShift", lua::wrap<&Level::game_setScreenShift>);
    m_script.registerFn("game_changeBg", lua::wrap<&Level::game_changeBg>);
    m_script.registerFn("game_setBonusLevel", lua::wrap<&Level::game_setBonusLevel>);
    m_script.registerFn("game_planAction", lua::wrap<&Level::game_planAction>);
    m_script.registerFn("game_isPlanning", lua::wrap<&Level::game_isPlanning>);
    m_script.registerFn("game_killPlan", lua::wrap<&Level::game_killPlan>);
    m_script.registerFn("game_hint", lua::wrap<&Level::game_hint>);
    m_script.registerFn("game_flashButton", lua::wrap<&Level::game_flashButton>);

    m_script.registerFn("model_addAnim", lua::wrap<&Level::model_addAnim>);
    m_script.registerFn("model_runAnim", lua::wrap<&Level::model_runAnim>);
    m_script.registerFn("model_setAnim", lua::wrap<&Level::model_setAnim>);
    m_script.registerFn("model_useSpecialAnim", lua::wrap<&Level::model_useSpecialAnim>);
    m_script.registerFn("model_setEffect", lua::wrap<&Level::model_setEffect>);
    m_script.registerFn("model_getLoc", lua::wrap<&Level::model_getLoc>);
    m_script.registerFn("model_initPos", lua::wrap<&Level::model_setLoc>);
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
    m_script.registerFn("model_goto", lua::wrap<&Level::model_goto>);
    m_script.registerFn("model_gotoRandom", lua::wrap<&Level::model_gotoRandom>);

    m_script.registerFn("sound_addSound", lua::wrap<&Level::sound_addSound>);
    m_script.registerFn("sound_playSound", lua::wrap<&Level::sound_playSound>);
    m_script.registerFn("sound_playMusic", lua::wrap<&Level::sound_playMusic>);
    m_script.registerFn("sound_stopMusic", lua::wrap<&Level::sound_stopMusic>);

    m_script.registerFn("level_createRoom", lua::wrap<&Level::level_createRoom>);
    m_script.registerFn("level_getRestartCounter", lua::wrap<&Level::level_getRestartCounter>);
    m_script.registerFn("level_getDepth", lua::wrap<&Level::level_getDepth>);
    m_script.registerFn("level_isNewRound", lua::wrap<&Level::level_isNewRound>);
    m_script.registerFn("level_isSolved", lua::wrap<&Level::level_isSolved>);
    m_script.registerFn("level_isReady", lua::wrap<&Level::level_isReady>);
    m_script.registerFn("level_action_save", lua::wrap<&Level::level_action_save>);
    m_script.registerFn("level_action_load", lua::wrap<&Level::level_action_load>);
    m_script.registerFn("level_action_restart", lua::wrap<&Level::level_action_restart>);
    m_script.registerFn("level_action_showMoves", lua::wrap<&Level::level_action_showMoves>);
    m_script.registerFn("level_save", lua::wrap<&Level::level_save>);
    m_script.registerFn("level_load", lua::wrap<&Level::level_load>);

    m_script.registerFn("slideshow_enter", lua::wrap<&Level::slideshow_enter>);
    m_script.registerFn("slideshow_exit", lua::wrap<&Level::slideshow_exit>);
    m_script.registerFn("slide_display", lua::wrap<&Level::slide_display>);

    m_script.registerFn("demo_enter", lua::wrap<&Level::demo_enter>);
    m_script.registerFn("demo_exit", lua::wrap<&Level::demo_exit>);
    m_script.registerFn("demo_state", lua::wrap<&Level::demo_state>);

    m_script.registerFn("dialog_isDialog", lua::wrap<&Level::dialog_isDialog>);
    m_script.registerFn("dialog_defineColor", lua::wrap<&Level::dialog_defineColor>);
    m_script.registerFn("dialog_add", lua::wrap<&Level::dialog_add>);

    m_script.registerFn("options_getInt", lua::wrap<&Level::options_getInt>);
}

void Level::level_createRoom(unsigned width, unsigned height, const std::string& bg) {
    m_layout = std::make_unique<LevelLayout>(*this, USize{width, height});
    m_screen.addImage(bg, "background");
}

int Level::level_getRestartCounter() const {
    return m_attempt;
}

int Level::level_getDepth() const {
    return m_record.depth;
}

bool Level::level_isNewRound() const {
    return m_roundFlag;
}

bool Level::level_isSolved() {
    return rules().solved();
}

bool Level::level_isReady() {
    return m_rules->ready();
}

bool Level::level_action_restart() {
    restart(true);
    return true;
}

bool Level::level_action_showMoves(std::string moves) {
    Log::verbose<Log::lua>("showMoves");
    m_rules->enqueue(moves, true);
    return true;
}

bool Level::level_action_save() {
    save(true);
    return true;
}

bool Level::level_action_load() {
    load(true);
    return true;
}

bool Level::level_save(const std::string& text_models) {
    saveFile()->write("saved_moves = '"s + m_replay + "'\n\nsaved_models = " + text_models);
    return true;
}

bool Level::level_load(const std::string& text_moves) {
    setBusy(BusyReason::loading); // unset in Level::tick()
    m_replay = text_moves;
    m_rules->enqueue(text_moves, true);
    return true;
}

void Level::slideshow_enter() {
    setBusy(BusyReason::slideshow);
}

void Level::slideshow_exit() {
    m_screen.display();
    m_screen.subs().clear();
    setBusy(BusyReason::slideshow, false);
}

void Level::slide_display(const std::string& filename, int x, int y) {
    auto imgData = decoders::png(m_instance, filename);
    if(x == 0 && y == 0) {
        m_screen.display(BufferImage::create(m_instance, imgData.size, TextureType::image, std::move(imgData.data)));
    } else {
        auto image = dynamic_cast<BufferImage*>(m_screen.displayed());
        image->compose(imgData, {x, y});
    }
}

void Level::demo_enter() {
    setBusy(BusyReason::demo);
}

void Level::demo_exit() {
    setBusy(BusyReason::demo, false);
}

bool Level::demo_state() {
    return inDemo();
}

void Level::game_setRoomWaves(float amplitude, float period, float speed) {
    m_screen.setWaves(amplitude, period, speed);
}

int Level::game_addModel(const std::string& type, int x, int y, const std::string& shape) {
    return layout().addModel(type, x, y, shape);
}

int Level::game_getCycles() {
    return timer().tickCount();
}

void Level::game_setBonusLevel(bool value) {
    Log::verbose<Log::lua>("setBonusLevel ", value);
    m_rules->bonusSwitch(value);
}

void Level::model_addAnim(int index, const std::string& name, const std::string& filename, std::optional<int> orientation) {
    auto* image = m_screen.addImage(filename);
    layout().getModel(index).anim().add(name, orientation.value_or(Model::Orientation::left), image);
}

void Level::model_runAnim(int index, const std::string& name, std::optional<int> phase) {
    layout().getModel(index).anim().set(name, phase.value_or(0), true);
}

void Level::model_setAnim(int index, const std::string& name, int phase) {
    layout().getModel(index).anim().set(name, phase, false);
}

void Level::model_useSpecialAnim(int index, const std::string& name, int phase) {
    layout().getModel(index).anim().setExtra(name, phase);
}

void Level::model_setEffect(int index, const std::string& name) {
    setModelEffect(layout().getModel(index), name);
}

void Level::setModelEffect(Model& model, const std::string& name) {
    model.setEffect(name, m_screen.timeAlive());
}

std::pair<int, int> Level::model_getLoc(int index) {
    auto [x, y] = layout().getModel(index).xyFinal();
    return {x, y};
}

void Level::model_setLoc(int index, int x, int y, bool left) {
    layout().getModel(index).setLoc({x, y}, left ? Model::Orientation::left : Model::Orientation::right);
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
        case Model::Action::willBusy:
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
    const auto& model = layout().getModel(index);
    if(!model.alive())
        return "dead";
    else if(model.talking())
        return "talking";
    else if(model.pushing())
        return "pushing";
    else
        return "normal";
}

int Level::model_getTouchDir(int index) {
    auto dir = layout().getModel(index).touchDir();
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
    return layout().getModel(index).alive();
}

bool Level::model_isAtBorder(int index) {
    return !!layout().borderDir(layout().getModel(index));
}

bool Level::model_isOut(int index) {
    return layout().isOut(layout().getModel(index));
}

bool Level::model_isLeft(int index) {
    return layout().getModel(index).orientation() == Model::Orientation::left;
}

unsigned Level::model_getW(int index) {
    return layout().getModel(index).size().width;
}

unsigned Level::model_getH(int index) {
    return layout().getModel(index).size().height;
}

void Level::model_setGoal(int index, const std::string& goal) {
    auto& model = layout().getModel(index);
    if(goal == "goal_alive")
        model.goal() = Model::Goal::alive;
    else if(goal == "goal_escape" || goal == "goal_out") { // FIXME: is there any need to differentiate?
        bool release = model.goal() == Model::Goal::alive;
        model.goal() = Model::Goal::escape;
        if(release)
            rules().checkEscape(model);
    } else
        model.goal() = Model::Goal::none;
}

void Level::model_change_turnSide(int index) {
    layout().getModel(index).turn();
}

void Level::model_setViewShift(int index, float dx, float dy, std::optional<float> speedX, std::optional<float> speedY) {
    auto [shift, speed] = layout().getModel(index).viewShift();
    shift = {dx, dy};
    speed = {speedX.value_or(0.f), speedY.value_or(0.f)};
}

std::pair<float, float> Level::model_getViewShift(int index) {
    auto [shift, speed] = layout().getModel(index).viewShift();
    return {shift.x, shift.y};
}

void Level::model_setBusy(int index, bool busy) {
    auto& model = layout().getModel(index);
    model.action() = busy
        ? model.moving() ? Model::Action::willBusy : Model::Action::busy
        : Model::Action::base;
    if(!busy)
        rules().checkEscape(model);
}

bool Level::model_isTalking(int index) {
    return layout().getModel(index).talking();
}

void Level::model_talk(int index, std::string name, std::optional<int> type, std::optional<bool> loops, std::optional<bool> dialogFlag) {
    constexpr std::array<AudioType, 3> types = {AudioType::talk, AudioType::sound, AudioType::music};
    if(isBusy(BusyReason::loading) || isBusy(BusyReason::replay))
        return;
    std::string param{};
    if(auto at = name.find('@'); at != std::string::npos) {
        param = name.substr(at + 1);
        name = name.substr(0, at);
    }
    auto& dialog = m_dialogs.at(name);
    auto lang = m_instance.persist().get("subtitles", ""s);
    if(dialog.texts.contains(lang)) {
        if(!param.empty()) {
            auto string = dialog.texts.at(lang);
            auto ix = string.find("%1");
            if(ix != std::string::npos)
                string.replace(ix, 2, param);
            else
                Log::error("replacement required but dialog text does not contain '%1'");
            m_screen.addSubtitle(string, dialog.colors);
        } else
            m_screen.addSubtitle(dialog.texts.at(lang), dialog.colors);
    }
    if(dialog.soundFile.empty()) {
        Log::error("Sound for dialog ID ", name, " not found!");
        return;
    }
    auto data = m_screen.addSound(name, dialog.soundFile, true);
    Log::verbose<Log::audio>("Audio type ", type.value_or(0));
    auto source = AudioSource::create(data, types[type.value_or(0)]);
    source->setRepeat(loops.value_or(false));
    source->setDialog(dialogFlag.value_or(false));
    if(index != -1 /* talk_both */)
        layout().getModel(index).talk() = source;
    else {
        auto [small, big] = rules().bothFish();
        small.talk() = source;
        big.talk() = source;
    }
    m_instance.audio().addSource(source);
}

void Level::model_killSound(int index) {
    killModelSound(layout().getModel(index));
}

void Level::killModelSound(Model& model) {
    const auto& talk = model.talk();
    if(talk)
        m_instance.audio().removeSource(talk);
    model.talk() = {};
}

bool Level::model_equals(int index, int x, int y) {
    ICoords xy{x, y};
    if(!xy.within({}, ICoords{layout().size(), 1}))
        return false;
    if(index != -1 /* free space */) {
        const auto& model = layout().getModel(index);
        return model.shape().covers(xy - model.xy());
    } else
        return layout().modelAt(xy) == nullptr;
}

bool Level::model_goto(int index, int x, int y) {
    if(!m_rules->ready())
        return false;
    Log::verbose<Log::lua>("model_goto");
    ICoords dest{x, y};
    auto& model = layout().getModel(index);
    auto path = layout().findPath(model, dest);
    if(path.empty())
        Log::error("model_goto requested but path from ", model.xy(), " to ", dest, " not found");
    m_rules->switchFish(&model);
    m_rules->enqueue(path, true);
    return true;
}

bool Level::model_gotoRandom(int index, int minDistance) {
    if(!m_rules->ready())
        return false;
    Log::verbose<Log::lua>("model_gotoRandom");
    auto& model = layout().getModel(index);
    if(!m_rules->isFree(model)) {
        Log::debug<Log::gotos>("gotoRandom: model not free.");
        return false;
    }
    auto path = layout().randomPath(model, minDistance);
    if(path.empty())
        return false;
    m_rules->switchFish(&model);
    m_rules->enqueue(path, true);
    return true;
}

void Level::sound_addSound(const std::string& name, const std::string& filename) {
    m_screen.addSound(name, filename);
}

void Level::sound_playSound(const std::string& name, std::optional<int> volume) {
    m_screen.playSound(name, (float)volume.value_or(100) / 100.f);
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

void Level::game_planAction(LuaCallback function) {
    m_plan.emplace_back(std::move(function));
}

void Level::game_killPlan() {
    killPlan();
}

void Level::game_addDecor(const std::string& type, int m1, int m2, int dx1, int dy1, int dx2, int dy2) {
    if(type == "rope")
        m_layout->addRope(m_layout->getModel(m1), m_layout->getModel(m2), {dx1, dy1}, {dx2, dy2});
    else
        Log::error("Unknown decor ", type);
}

void Level::game_setScreenShift(float dx, float dy) {
    m_screen.setShift({dx, dy});
}

void Level::game_changeBg(const std::string& filename) {
    m_screen.replaceImage("background", filename);
}

bool Level::dialog_isDialog() {
    return m_instance.audio().isDialog();
}

void Level::dialog_defineColor(const std::string& name, int r1, int g1, int b1, std::optional<int> r2, std::optional<int> g2, std::optional<int> b2) {
    m_screen.subs().defineColors(name, {r1, g1, b1}, {r2.value_or(r1), g2.value_or(g1), b2.value_or(b1)});
}

void Level::dialog_add(const std::string& name, const std::string& color, std::map<std::string, std::string>&& subtitles) {
    std::string soundFile;
    if(std::string fnLevel = "sound/"s + m_record.codename + "/" + name + ".ogg"; m_instance.files().system(fnLevel)->exists())
        soundFile = std::move(fnLevel);
    else if(std::string fnShared = "sound/share/" + name + ".ogg"; m_instance.files().system(fnShared)->exists())
        soundFile = std::move(fnShared);
    if(!soundFile.empty())
        Log::verbose<Log::audio>("Using sound file ", soundFile, " for dialog ID ", name);
    else
        Log::verbose<Log::audio>("Sound for dialog ID ", name, " not found");
    m_dialogs.insert({name, {soundFile, color, std::move(subtitles)}});
}

int Level::options_getInt(const std::string& name) {
    if(name == "volume_music")
        return int(m_instance.audio().getVolume(AudioType::music) * 100);
    else
        return m_instance.persist().get(name, 0);
}

void Level::game_hint(const std::string& dialogName) {
    if(!dialogName.empty()) {
        auto& dialog = m_dialogs.at(dialogName);
        auto lang = m_instance.persist().get("subtitles", ""s);
        if(lang.empty())
            lang = "cs";
        if(dialog.texts.contains(lang))
            m_screen.showHint(dialog.texts.at(lang));
    } else
        m_screen.hideHint();
}

void Level::game_flashButton(const std::string& which) {
    if(which == "save")
        input().flashButton(Key::save);
    else if(which == "load")
        input().flashButton(Key::load);
    else if(which == "restart")
        input().flashButton(Key::restart);
    else
        Log::error("game_flashButton unknown button: ", which);
}
