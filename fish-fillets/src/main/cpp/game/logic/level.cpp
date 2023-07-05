#include "level.h"
#include "game/screens/levelscreen.h"
#include "subsystem/persist.h"

Level::Level(Instance& instance, LevelScreen& screen, LevelRecord& record) :
        m_instance(instance),
        m_screen(screen),
        m_record(record),
        m_script(instance, *this),
        m_attempt(0),
        m_roundFlag(false),
        m_goto(false)
{
    registerCallbacks();
    m_script.loadFile("script/globals.lua");
}

Level::~Level() {
    if(!inReplay()) {
        auto time = m_instance.persist().get("playtime", 0);
        time += (int)m_screen.timeAlive();
        Log::info<Log::lifecycle>("playtime ", time);
        m_instance.persist().set("playtime", time);
    }
}

LevelInput& Level::input() {
    return dynamic_cast<LevelInput&>(screen().input());
}

void Level::init() {
    m_attempt++;
    Log::info<Log::lifecycle>("Level ", m_record.codename, ", attempt ", m_attempt);
    m_script.loadFile(m_record.script_filename);
    m_rules = std::make_unique<LevelRules>(*this, layout());
    input().setSavePossible(savePossible());
    input().setLoadPossible(loadPossible());
}

void Level::update(float dt) {
    layout().animate(dt, isBusy(BusyReason::loading)
            ? LevelLayout::speed_loading
            : rules().isVintage()
            ? LevelLayout::speed_instant
            : LevelLayout::speed_normal);
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
    if(!isBusy(BusyReason::loading) && !isBusy(BusyReason::slideshow))
        m_script.doString("script_update()");
    if(!m_tickSchedule.empty()) {
        if(m_tickSchedule.front()())
            m_tickSchedule.pop_front();
    }
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

bool Level::inDemo() const {
    return isBusy(BusyReason::demo);
}

void Level::skipBusy() {
    if(m_busy[BusyReason::poster])
        m_screen.exit();
    else if(m_busy[BusyReason::slideshow])
        quitSlideshow();
    else if(m_busy[BusyReason::loading] && !inDemo())
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

bool Level::inGoTo() const {
    return m_goto;
}

void Level::skipGoTo(bool finish) {
    if(finish)
        runScheduledAll();
    else
        m_rules->clearQueue();
    m_goto = false;
}

void Level::runScheduledAll() {
    while(!m_rules->ready()) {
        for(auto& transition : m_transitions)
            transition.callback();
        m_transitions.clear();
        layout().animate(0.f, LevelLayout::speed_instant);
        rules().update();
    }
}

void Level::recordMove(char key) {
    m_replay += key;
}

void Level::notifyRound() {
    m_roundFlag = true;
}

void Level::notifyFish(Model::Fish fish) {
    Log::debug<Log::motion>("Active fish: ", fish == Model::Fish::small ? "small" : fish == Model::Fish::big ? "big" : "none");
    if(accepting())
        input().setFish(fish);
}

void Level::notifyDeath() {
    input().setSavePossible(false);
}

void Level::notifyEscape(Model* model) {
    m_script.doString("notify_escape(" + std::to_string(model->index()) + ")");
}

bool Level::quitSlideshow() {
    if(!isBusy(BusyReason::slideshow))
        return false;
    model_killSound(1); /* actor_index used in demo_briefcase.lua */
    m_tickSchedule.clear();
    slideshow_exit();
    return true;
}

void Level::save(bool force) {
    if(savePossible() || force) {
        m_script.doString("script_save()");
        if(m_busy.none())
            input().setLoadPossible(true);
        m_screen.saveEffect();
    }
}

void Level::load(bool keepSchedule) {
    if(loadPossible() || keepSchedule) {
        killDialogsHard();
        m_tickSchedule.clear();
        m_tickSchedule.emplace_back([&, contents = saveFile()->read(), keepSchedule]() {
            reinit(keepSchedule);
            m_script.doString(contents);
            m_script.doString("script_load()");
            return true;
        });
    }
}

void Level::success() {
    m_tickSchedule.emplace_back([&]() {
        if(m_tickSchedule.size() == 1 && !m_instance.audio().isDialog()) {
            if(!isBusy(BusyReason::replay)) {
                m_record.solved = true;
                solveFile()->write("saved_moves = '"s + m_replay + "'\n");
            }
            if(record().script_ending.empty())
                m_screen.exit();
            else {
                m_script.loadFile(record().script_ending);
                setBusy(BusyReason::poster);
            }
        }
        else
            m_tickSchedule.push_back(std::move(m_tickSchedule.front()));
        return true;
    });
}

void Level::replay() {
    input().setRestartPossible(false);
    setBusy(BusyReason::replay);
    m_tickSchedule.emplace_back([&, contents = solveFile()->read()]() {
        m_script.doString(contents);
        m_script.doString("saved_models = {{}}");
        m_script.doString("script_load()");
        return true;
    });
}

bool Level::inReplay() const {
    return isBusy(BusyReason::replay);
}

void Level::restart(bool keepSchedule) {
    killDialogsHard();
    m_tickSchedule.clear();
    m_tickSchedule.emplace_back([&, keepSchedule]() {
        reinit(keepSchedule);
        return true;
    });
}

void Level::restartWhenEmpty() {
    m_tickSchedule.emplace_back([&]() {
        Log::debug<Log::lua>("restartWhenEmpty waiting callbacks: ", m_tickSchedule.size());
        if(m_tickSchedule.size() == 1 && !m_instance.audio().isDialog()) {
            m_screen.subs().clear();
            m_screen.killSounds();
            reinit();
        }
        else
            m_tickSchedule.push_back(std::move(m_tickSchedule.front()));
        return true;
    });
}

void Level::reinit(bool keepSchedule) {
    m_dialogs.clear();
    m_screen.restore();
    init();
    m_transitions.clear();
    m_replay.clear();
    m_timer.reset();
    setBusy(BusyReason::slideshow, false);
    setBusy(BusyReason::loading, false);
    setBusy(BusyReason::poster, false);
    m_goto = false;
    if(!keepSchedule)
        setBusy(BusyReason::demo, false);
}

bool Level::savePossible() const {
    return m_busy.none() && rules().solvable();
}

bool Level::loadPossible() const {
    return saveFile()->exists() && !inDemo();
}

std::unique_ptr<IFile> Level::saveFile() const {
    return m_instance.files().user(m_record.saveFilename());
}

std::unique_ptr<IFile> Level::solveFile() const {
    return m_instance.files().user(m_record.solveFilename());
}

bool Level::scheduleGoTo(ICoords coords) {
    return scheduleGoTo(rules().activeFish_model(), coords);
}

bool Level::scheduleGoTo(Model* unit, ICoords coords) {
    if(!m_rules->isFree(unit)) {
        Log::debug<Log::gotos>("GoTo rejected, model not free.");
        return false;
    }
    auto path = layout().findPath(unit, coords);
    if(!path.empty()) {
        m_goto = true;
        m_rules->enqueue(path, false);
        m_tickSchedule.emplace_back([&]() {
            if(!m_rules->ready())
                return false;
            m_goto = false;
            return true;
        });
        return true;
    } else
        return false;
}
