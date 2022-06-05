#include "level.h"
#include "game/screens/levelscreen.h"

Level::Level(Instance& instance, LevelScreen& screen, LevelRecord& record) :
        m_instance(instance),
        m_screen(screen),
        m_record(record),
        m_script(instance, *this),
        m_attempt(0),
        m_roundFlag(false)
{
    registerCallbacks();
}

LevelInput& Level::input() {
    return dynamic_cast<LevelInput&>(screen().input());
}

void Level::init() {
    m_attempt++;
    Log::info("Level ", m_record.codename, ", attempt ", m_attempt);
    m_script.loadFile(m_instance.files().system(m_record.script_filename));
    m_rules = std::make_unique<LevelRules>(*this, layout());
    input().setSavePossible(savePossible());
    input().setLoadPossible(loadPossible());
}

void Level::update(float dt) {
    layout().animate(dt, isBusy(BusyReason::loading) ? LevelLayout::speed_loading : LevelLayout::speed_normal);
    rules().update();
    if(m_timer.ticked())
        tick();
}

void Level::tick() {
    for(auto* model : layout().models())
        model->anim().update();
    for(auto& transition : m_transitions) {
        if(--transition.countdown == 0)
            transition.callback();
    }
    m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(), [](const auto& transition) { return transition.countdown == 0; }), m_transitions.end());
    if(!isBusy(BusyReason::loading) && !isBusy(BusyReason::demo))
        m_script.doString("script_update();");
    if(!m_tickSchedule.empty()) {
        if(m_tickSchedule.front()())
            m_tickSchedule.pop_front();
    }
    if(isBusy(BusyReason::demo) && m_tickSchedule.empty())
        quitDemo();
    m_roundFlag = false;
}

void Level::setBusy(BusyReason reason, bool busy) {
    m_busy[reason] = busy;
    if(m_busy.any())
        input().setFish(Model::Fish::none);
    else
        input().setFish(rules().activeFish());
    input().setSavePossible(savePossible());
    input().setLoadPossible(loadPossible());
}

bool Level::isBusy(BusyReason reason) const {
    return m_busy[reason];
}

void Level::skipBusy() {
    if(m_busy[BusyReason::demo])
        quitDemo();
    else if(m_busy[BusyReason::loading])
        runScheduledAll();
}

void Level::transition(int frames, std::function<void()>&& callback) {
    if(!isBusy(BusyReason::loading))
        m_transitions.push_back({frames, std::move(callback)});
    else
        callback();
}

bool Level::transitioning() const {
    return !m_transitions.empty();
}

bool Level::loading() const {
    return isBusy(BusyReason::loading);
}

bool Level::accepting() const {
    return m_busy.none();
}

void Level::schedule(Callback&& action) {
    m_moveSchedule.push_back(std::move(action));
}

void Level::scheduleBlocking(Callback&& action) {
    schedule(std::move(action));
    setBusy(BusyReason::schedule);
}

bool Level::runScheduled() {
    if(!m_moveSchedule.empty()) {
        bool ret = m_moveSchedule.front()();
        if(ret) {
            m_moveSchedule.pop_front();
            if(m_moveSchedule.empty())
                setBusy(BusyReason::schedule, false);
        }
        return ret;
    } else
        return false;
}

void Level::runScheduledAll() {
    while(!m_moveSchedule.empty()) {
        layout().animate(0.f, LevelLayout::speed_instant);
        rules().update();
    }
}

void Level::clearSchedule() {
    m_moveSchedule.clear();
    setBusy(BusyReason::schedule, false);
}

void Level::recordMove(char key) {
    m_replay += key;
}

void Level::notifyRound() {
    m_roundFlag = true;
}

void Level::notifyFish(Model::Fish fish) {
    Log::info("Active fish: ", fish == Model::Fish::small ? "small" : fish == Model::Fish::big ? "big" : "none");
    if(accepting())
        input().setFish(fish);
}

void Level::notifyDeath() {
    input().setSavePossible(false);
}

void Level::notifySolved() {
    m_record.solved = true;
    solveFile().write("saved_moves = '"s + m_replay + "'\n");
    m_screen.exit();
}

bool Level::quitDemo() {
    if(isBusy(BusyReason::demo)) {
        model_killSound(1); /* actor_index used in demo_briefcase.lua */
        m_tickSchedule.clear();
        m_screen.display("");
        m_screen.subs().clear();
        setBusy(BusyReason::demo, false);
        return true;
    } else
        return false;
}

void Level::save() {
    if(savePossible()) {
        m_script.doString("script_save()");
        if(m_busy.none())
            input().setLoadPossible(true);
        m_screen.saveEffect();
    }
}

void Level::load(bool keepSchedule) {
    if(loadPossible()) {
        auto file = saveFile();
        killDialogsHard();
        m_tickSchedule.clear();
        m_tickSchedule.emplace_back([&, file, keepSchedule]() {
            reinit(keepSchedule);
            m_script.loadFile(file);
            m_script.doString("script_load()");
            return true;
        });
    }
}

void Level::restart(bool keepSchedule) {
    killDialogsHard();
    m_tickSchedule.clear();
    m_tickSchedule.emplace_back([&, keepSchedule]() {
        reinit(keepSchedule);
        return true;
    });
}

void Level::reinit(bool keepSchedule) {
    m_dialogs.clear();
    m_screen.restore();
    init();
    m_transitions.clear();
    m_replay.clear();
    setBusy(BusyReason::demo, false);
    setBusy(BusyReason::loading, false);
    if(!keepSchedule)
        clearSchedule();
}

bool Level::savePossible() const {
    return !isBusy(BusyReason::loading) && !isBusy(BusyReason::demo) && !isBusy(BusyReason::schedule) && rules().solvable();
}

bool Level::loadPossible() const {
    return saveFile().exists();
}

UserFile Level::saveFile() const {
    return m_instance.files().user(m_record.saveFilename());
}

UserFile Level::solveFile() const {
    return m_instance.files().user(m_record.solveFilename());
}

bool Level::scheduleGoTo(ICoords coords) {
    auto* unit = rules().activeFish_model();
    auto path = layout().findPath(unit, coords);
    if(!path.empty()) {
        setBusy(BusyReason::longpress);
        for(auto dir : path) {
            schedule([&, dir]() {
                m_rules->keyInput(Input::toKey(dir));
                return true;
            });
        }
        schedule([&]() {
            setBusy(BusyReason::longpress, false);
            return true;
        });
        return true;
    } else
        return false;
}