#include "level.h"
#include "game/screens/levelscreen.h"

Level::Level(Instance& instance, LevelScreen& screen, const LevelRecord& record) :
        m_instance(instance),
        m_screen(screen),
        m_record(record),
        m_script(instance, *this),
        m_loading(false),
        m_inDemo(false)
{
    registerCallbacks();
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
    if(!m_inDemo & !m_loading)
        m_script.doString("script_update();");
    if(!m_tickSchedule.empty()) {
        if(m_tickSchedule.front()())
            m_tickSchedule.pop_front();
    }
    if(m_inDemo && m_tickSchedule.empty())
        quitDemo();
}

void Level::blockFor(int frames, std::function<void()>&& callback) {
    if(!m_loading)
        m_blocks.push_back({frames, std::move(callback)});
    else
        callback();
}

bool Level::blocked() const {
    return !m_blocks.empty();
}

bool Level::accepting() const {
    return !blocked() && m_moveSchedule.empty() && !m_inDemo && !m_loading;
}

void Level::scheduleAction(std::function<bool()>&& action) {
    m_moveSchedule.emplace_back(std::move(action));
}

bool Level::runScheduled() {
    if(!m_moveSchedule.empty()) {
        bool ret = m_moveSchedule.front()();
        if(ret)
            m_moveSchedule.pop_front();
        return ret;
    } else
        return false;
}

void Level::recordMove(char key) {
    m_replay += key;
}

bool Level::quitDemo() {
    if(m_inDemo) {
        model_killSound(1); /* actor_index used in demo_briefcase.lua */
        m_tickSchedule.clear();
        m_screen.display("");
        m_screen.m_subs.clear();
        m_inDemo = false;
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
    m_moveSchedule.clear();
    level_action_load();
}

void Level::restart() {
    killDialogsHard();
    m_moveSchedule.clear();
    level_action_restart();
}