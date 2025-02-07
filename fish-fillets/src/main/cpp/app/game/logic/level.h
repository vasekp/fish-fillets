#ifndef FISH_FILLETS_LEVEL_H
#define FISH_FILLETS_LEVEL_H

#include "common.h"
#include "instance.h"
#include "subsystem/script.h"
#include "game/structure/levelrecord.h"
#include "game/logic/timer.h"
#include "game/logic/model.h"
#include "utils/enumbitset.h"

class LevelScreen;
class LevelInput;
class LevelLayout;
class LevelRules;
class IFile;

class Level : public ScriptReferrer {
public:
    Level(Instance& instance, LevelScreen& screen, LevelRecord& record);
    ~Level();

    Instance& instance() { return m_instance; }
    LevelLayout& layout() { return *m_layout; }
    LevelRules& rules() { return *m_rules; }
    const LevelRules& rules() const { return *m_rules; }
    const LevelRecord& record() { return m_record; }
    LevelScreen& screen() { return m_screen; }
    Timer& timer() { return m_timer; }
    LevelInput& input();

    void init();
    void reinit(bool fromScript = false, bool bumpAttempt = true);
    void update(std::chrono::duration<float> dt);
    void tick();
    void save(bool fromScript = false);
    void load(bool fromScript = false);
    bool savePossible() const;
    bool loadPossible() const;
    void restart(bool fromScript = false);
    void restartWhenEmpty();
    void replay();
    void success();
    bool undo();

    void transition(int frames, std::function<void()>&& callback);
    bool transitioning() const;
    bool loading() const;
    bool enqueueGoTo(ICoords coords);
    bool enqueueGoTo(Model& unit, ICoords coords);
    void dispatchMoveQueue();
    void recordMove(char key);
    bool accepting() const;
    void skipBusy();
    bool inDemo() const;
    bool inSlideshow() const;
    bool inGoTo() const;
    bool inReplay() const;
    void skipGoTo(bool finish);
    bool quitSlideshow();
    bool activeFishReady() const;

    void killModelSound(Model& model);
    void killPlan();
    void setModelEffect(Model& model, const std::string& name);

    void notifyRound();
    void notifyFish(Model::Fish fish);
    void notifyDeath(Model& unit);
    void notifyEscape(Model& model);

private:
    Instance& m_instance;
    LevelScreen& m_screen;
    LevelRecord& m_record;
    Timer m_timer;
    Script m_script;
    std::unique_ptr<LevelLayout> m_layout;
    std::unique_ptr<LevelRules> m_rules;
    std::string m_replay;
    int m_attempt;
    bool m_roundFlag;

    std::deque<Callback> m_plan;

    struct Delayed {
        int countdown;
        std::function<void()> callback;
    };
    std::vector<Delayed> m_transitions;

    struct Dialog {
        std::string soundFile;
        std::string colors;
        std::map<std::string, std::string> texts;
    };
    std::map<std::string, Dialog> m_dialogs;

    enum class BusyReason {
        loading,
        slideshow,
        demo,
        replay,
        SIZE
    };
    util::EnumBitset<BusyReason> m_busy;
    bool m_goto;

    struct UndoConds {
        std::chrono::steady_clock::time_point time;
        std::string replay;
        Model::Fish active;
    };
    std::optional<UndoConds> m_undo;

    void registerCallbacks();

    void level_createRoom(unsigned width, unsigned height, const std::string& bg);
    int level_getRestartCounter() const;
    int level_getDepth() const;
    bool level_isNewRound() const;
    bool level_isSolved();
    bool level_isReady();
    bool level_action_save();
    bool level_action_load();
    bool level_action_restart();
    bool level_action_showMoves(std::string moves);
    bool level_save(const std::string& text_models);
    bool level_load(const std::string& text_moves);
    void demo_enter();
    void demo_exit();
    bool demo_state();
    void slideshow_enter();
    void slideshow_exit();
    void slide_display(const std::string& filename, int x, int y);
    void game_setRoomWaves(float amplitude, float period, float speed);
    int game_addModel(const std::string& type, int x, int y, const std::string& shape);
    int game_getCycles();
    bool game_isPlanning();
    void game_planAction(LuaCallback function);
    void game_killPlan();
    void game_addDecor(const std::string& type, int m1, int m2, int dx1, int dy1, int dx2, int dy2);
    void game_setScreenShift(float dx, float dy);
    void game_changeBg(const std::string& filename);
    void game_setBonusLevel(bool value);
    void game_hint(const std::string& dialogName);
    void game_flashButton(const std::string& which);
    void model_addAnim(int index, const std::string& name, const std::string& filename,
                             std::optional<int> orientation);
    void model_runAnim(int index, const std::string& name, std::optional<int> phase);
    void model_setAnim(int index, const std::string& name, int phase);
    void model_useSpecialAnim(int index, const std::string& name, int phase);
    void model_setEffect(int index, const std::string& name);
    std::pair<int, int> model_getLoc(int index);
    void model_setLoc(int index, int x, int y, bool left);
    std::string model_getAction(int index);
    std::string model_getState(int index);
    int model_getTouchDir(int index);
    bool model_isAlive(int index);
    bool model_isOut(int index);
    bool model_isLeft(int index);
    bool model_isAtBorder(int index);
    unsigned model_getW(int index);
    unsigned model_getH(int index);
    void model_setGoal(int index, const std::string& goal);
    void model_change_turnSide(int index);
    void model_setViewShift(int index, float dx, float dy, std::optional<float> speedX, std::optional<float> speedY);
    std::pair<float, float> model_getViewShift(int index);
    void model_setBusy(int index, bool busy);
    bool model_isTalking(int index);
    void model_talk(int index, std::string name, std::optional<int> type, std::optional<bool> loops, std::optional<bool> dialogFlag);
    void model_killSound(int index);
    bool model_equals(int index, int x, int y);
    bool model_goto(int index, int x, int y);
    bool model_gotoRandom(int index, int minDistance);
    void sound_addSound(const std::string& name, const std::string& filename);
    void sound_playSound(const std::string& name, std::optional<int> volume);
    void sound_playMusic(const std::string& filename);
    void sound_stopMusic();
    bool dialog_isDialog();
    void dialog_defineColor(const std::string& name, int r1, int g1, int b1, std::optional<int> r2, std::optional<int> g2, std::optional<int> b2);
    void dialog_add(const std::string& name, const std::string& color, std::map<std::string, std::string>&& subtitles);
    int options_getInt(const std::string& name);

    void setBusy(BusyReason reason, bool busy = true);
    bool isBusy(BusyReason reason) const;

    std::unique_ptr<IFile> saveFile() const;
    std::unique_ptr<IFile> solveFile() const;

    void saveUndo();
    void killUndo();
};

#endif //FISH_FILLETS_LEVEL_H
