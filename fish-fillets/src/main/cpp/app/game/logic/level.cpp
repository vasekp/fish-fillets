#include "level.h"
#include "layout.h"
#include "rules.h"
#include "game/screens/levelscreen.h"
#include "game/screens/poster.h"
#include "game/screens/screenmanager.h"
#include "subsystem/files.h"
#include "subsystem/persist.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"

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

void Level::reinit(bool fromScript) {
    m_dialogs.clear();
    m_screen.killSounds();
    m_screen.reset();
    init();
    m_transitions.clear();
    m_replay.clear();
    m_timer.reset();
    setBusy(BusyReason::slideshow, false);
    setBusy(BusyReason::loading, false);
    m_goto = false;
    if(!fromScript)
        setBusy(BusyReason::demo, false);
}


void Level::update(std::chrono::duration<float> dt) {
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
    for(auto& model : layout().models())
        model.anim().update();
    for(auto& transition : m_transitions) {
        if(--transition.countdown == 0)
            transition.callback();
    }
    m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(), [](const auto& transition) { return transition.countdown == 0; }), m_transitions.end());
    if(isBusy(BusyReason::loading) && m_rules->ready()) {
        m_script.doString("script_loadState()");
        setBusy(BusyReason::loading, false);
    }
    if(!isBusy(BusyReason::loading) && !isBusy(BusyReason::slideshow))
        m_script.doString("script_update()");
    if(!isBusy(BusyReason::loading) && !m_plan.empty()) {
        if(m_plan.front()())
            m_plan.pop_front();
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

bool Level::inSlideshow() const {
    return isBusy(BusyReason::slideshow);
}

bool Level::activeFishReady() const {
    auto* unit = m_rules->activeFish_model();
    if(!unit || unit->driven() || unit->action() == Model::Action::busy)
        return false;
    else
        return true;
}

void Level::skipBusy() {
    if(m_busy[BusyReason::slideshow])
        quitSlideshow();
    else if(m_busy[BusyReason::loading] && !inDemo()) {
        m_script.doString("script_loadState()");
        m_rules->skipLoad();
        setBusy(BusyReason::loading, false);
    }
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
        dispatchMoveQueue();
    else
        m_rules->clearQueue();
    m_goto = false;
}

void Level::dispatchMoveQueue() {
    while(!m_rules->ready()) {
        for(auto& transition : m_transitions)
            transition.callback();
        m_transitions.clear();
        layout().animate({}, LevelLayout::speed_instant);
        rules().update();
    }
}

void Level::recordMove(char key) {
    if(!isBusy(BusyReason::loading))
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

void Level::notifyDeath(Model& unit) {
    input().setSavePossible(false);
    screen().playSound(unit.supportType() == Model::SupportType::small ? "dead_small" : "dead_big");
    killModelSound(unit);
    auto [small, big] = rules().bothFish();
    small.action() = Model::Action::base;
    big.action() = Model::Action::base;
    if(!inDemo()) {
        killPlan();
        if(!small.alive() && !big.alive())
            transition(ModelAnim::framesRestart, [this]() { restartWhenEmpty(); });
    }
}

void Level::notifyEscape(Model& model) {
    m_script.doString("notify_escape(" + std::to_string(model.index()) + ")");
}

bool Level::quitSlideshow() {
    if(!isBusy(BusyReason::slideshow))
        return false;
    model_killSound(1); /* actor_index used in demo_briefcase.lua */
    m_plan.clear();
    slideshow_exit();
    return true;
}

void Level::save(bool fromScript) {
    if(savePossible() || fromScript) {
        m_script.doString("script_save()");
        if(m_busy.none())
            input().setLoadPossible(true);
        m_screen.saveEffect();
    }
}

void Level::load(bool fromScript) {
    if(loadPossible() || fromScript) {
        if(!fromScript)
            killPlan();
        reinit(fromScript);
        m_script.doString(saveFile()->read());
        m_script.doString("script_load()");
    }
}

void Level::success() {
    m_plan.emplace_back([&]() {
        if(m_instance.audio().isDialog())
            return false;
        if(!isBusy(BusyReason::replay)) {
            m_record.solved = true;
            solveFile()->write("saved_moves = '"s + m_replay + "'\n");
        }
        if(record().script_ending.empty())
            m_screen.exit();
        else
            m_instance.screens().poster(record());
        return true;
    });
}

void Level::replay() {
    input().setRestartPossible(false);
    setBusy(BusyReason::replay);
    m_script.doString(solveFile()->read());
    m_script.doString("saved_models = {{}}");
    m_script.doString("script_load()");
}

bool Level::inReplay() const {
    return isBusy(BusyReason::replay);
}

void Level::restart(bool fromScript) {
    if(!fromScript)
        killPlan();
    reinit(fromScript);
}

void Level::restartWhenEmpty() {
    m_plan.emplace_back([&]() {
        if(m_instance.audio().isDialog())
            return false;
        reinit();
        return true;
    });
}

void Level::killPlan() {
    m_plan.clear();
}

bool Level::savePossible() const {
    return m_busy.none() && rules().solvable();
}

bool Level::loadPossible() const {
    return saveFile()->exists() && !inDemo() && !inReplay();
}

std::unique_ptr<IFile> Level::saveFile() const {
    return m_instance.files().user(m_record.saveFilename());
}

std::unique_ptr<IFile> Level::solveFile() const {
    return m_instance.files().user(m_record.solveFilename());
}

bool Level::enqueueGoTo(ICoords coords) {
    return enqueueGoTo(*rules().activeFish_model(), coords);
}

bool Level::enqueueGoTo(Model& unit, ICoords coords) {
    if(!m_rules->isFree(unit)) {
        Log::debug<Log::gotos>("GoTo rejected, model not free.");
        return false;
    }
    auto path = layout().findPath(unit, coords);
    if(!path.empty()) {
        m_goto = true;
        m_rules->enqueue(path, false);
        m_plan.emplace_back([&]() {
            if(!m_rules->ready())
                return false;
            m_goto = false;
            return true;
        });
        return true;
    } else
        return false;
}
