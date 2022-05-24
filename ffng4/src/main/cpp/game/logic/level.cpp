#include "level.h"
#include "game/screens/levelscreen.h"

Level::Level(Instance& instance, LevelScreen& screen, const LevelRecord& record) :
        m_instance(instance),
        m_screen(screen),
        m_record(record),
        m_script(instance, *this)
{
    registerCallbacks();
}

LevelInput &Level::input() {
    return m_screen.m_input;
}

void Level::init() {
    m_script.loadFile(m_instance.files().system(m_record.script_filename));
    m_rules = std::make_unique<LevelRules>(*this, layout());
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
}

void Level::setBusy(BusyReason reason, bool busy) {
    m_busy[reason] = busy;
    if(m_busy.any()) {
        input().setFish(Model::Fish::none);
        input().setSavePossible(false);
        input().setLoadPossible(false);
    } else {
        input().setFish(rules().activeFish());
        input().setSavePossible(savePossible());
        input().setLoadPossible(loadPossible());
    }
}

bool Level::isBusy(BusyReason reason) const {
    return m_busy[reason];
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

bool Level::accepting() const {
    return m_busy.none();
}

void Level::schedule(Callback&& action) {
    m_moveSchedule.push_back(std::move(action));
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

void Level::clearSchedule() {
    m_moveSchedule.clear();
    setBusy(BusyReason::schedule, false);
}

void Level::recordMove(char key) {
    m_replay += key;
}

void Level::notifyFish(Model::Fish fish) {
    if(accepting())
        input().setFish(fish);
}

bool Level::quitDemo() {
    if(isBusy(BusyReason::demo)) {
        model_killSound(1); /* actor_index used in demo_briefcase.lua */
        m_tickSchedule.clear();
        m_screen.display("");
        m_screen.m_subs.clear();
        setBusy(BusyReason::demo, false);
        return true;
    } else
        return false;
}

void Level::save() {
    level_action_save();
}

void Level::load() {
    // TODO check file exists
    killDialogsHard();
    clearSchedule();
    level_action_load();
}

void Level::restart() {
    killDialogsHard();
    clearSchedule();
    level_action_restart();
}

bool Level::savePossible() const {
    return accepting() && rules().solvable();
}

bool Level::loadPossible() const {
    return saveFile().exists();
}

UserFile Level::saveFile() const {
    return m_instance.files().user("saves/"s + m_record.codename + ".lua");
}