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
    for(auto& block : m_blocks) {
        if(--block.countdown == 0)
            block.callback();
    }
    m_blocks.erase(std::remove_if(m_blocks.begin(), m_blocks.end(), [](const auto& block) { return block.countdown == 0; }), m_blocks.end());
    if(m_blocks.empty())
        setBusy(BusyReason::blocked, false);
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
    m_busy[(std::size_t)reason] = busy;
    if(isBusy(BusyReason::loading) || isBusy(BusyReason::demo) || isBusy(BusyReason::schedule))
        input().setFish(Model::Fish::none);
    else
        input().setFish(rules().activeFish());
}

bool Level::isBusy(BusyReason reason) const {
    return m_busy[(std::size_t)reason];
}

void Level::blockFor(int frames, std::function<void()>&& callback) {
    if(!isBusy(BusyReason::loading)) {
        m_blocks.push_back({frames, std::move(callback)});
        setBusy(BusyReason::blocked);
    } else
        callback();
}

bool Level::blocked() const {
    return isBusy(BusyReason::blocked);
}

void Level::clearBlocks() {
    m_blocks.clear();
    setBusy(BusyReason::blocked, false);
}

bool Level::accepting() const {
    return !m_busy.any();
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