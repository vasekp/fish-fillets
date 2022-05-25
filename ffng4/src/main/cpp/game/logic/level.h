#ifndef FISH_FILLETS_LEVEL_H
#define FISH_FILLETS_LEVEL_H

#include "subsystem/script.h"
#include "game/structure/levelrecord.h"
#include "layout.h"
#include "rules.h"

#include <functional>
#include <variant>

class LevelScreen;
class LevelInput;

class Level : public ScriptReferrer {
    Instance& m_instance;
    LevelScreen& m_screen;
    const LevelRecord& m_record;
    Script m_script;
    std::unique_ptr<LevelLayout> m_layout;
    std::unique_ptr<LevelRules> m_rules;
    std::string m_replay;
    int m_attempt;

    std::deque<Callback> m_tickSchedule;
    std::deque<Callback> m_moveSchedule;

    struct Delayed {
        int countdown;
        std::function<void()> callback;
    };
    std::vector<Delayed> m_transitions;

    struct Dialog {
        std::string text;
        std::string colors;
        std::string soundFile;
    };
    std::map<std::string, Dialog> m_dialogs;

    enum class BusyReason {
        loading,
        schedule,
        demo,
        SIZE
    };
    EnumBitset<BusyReason> m_busy;

public:
    Level(Instance& instance, LevelScreen& screen, const LevelRecord& record);

    LevelLayout& layout() { return *m_layout; }
    LevelRules& rules() { return *m_rules; }
    const LevelRules& rules() const { return *m_rules; }
    LevelScreen& screen() { return m_screen; }
    LevelInput& input();

    void init();
    void reinit(bool keepSchedule = false);
    void tick();
    void save();
    void load(bool keepSchedule = false);
    void restart(bool keepSchedule = false);
    bool savePossible() const;
    bool loadPossible() const;
    UserFile saveFile() const;

    void transition(int frames, std::function<void()>&& callback);
    bool transitioning() const;
    void schedule(Callback&& action);
    bool runScheduled();
    void recordMove(char key);
    bool quitDemo();
    bool accepting() const;

    void playSound(const std::string& name, float volume = 1.f);
    void killModelSound(Model* model);
    void killDialogs();
    void killDialogsHard();
    void setModelEffect(Model* model, const std::string& name);
    void notifyFish(Model::Fish fish);

private:
    void registerCallbacks();

    void level_createRoom(int width, int height, const std::string& bg);
    int level_getRestartCounter();
    int level_getDepth() const;
    bool level_isNewRound() const;
    bool level_isSolved();
    void level_planShow(LuaCallback function);
    bool level_isShowing();
    bool level_action_move(const std::string& move);
    bool level_action_save();
    bool level_action_load();
    bool level_action_restart();
    bool level_save(const std::string& text_models);
    bool level_load(const std::string& text_moves);
    void level_newDemo(const std::string& filename);
    void demo_display(const std::string& filename);
    void game_setRoomWaves(float amplitude, float period, float speed);
    int game_addModel(const std::string& type, int x, int y, const std::string& shape);
    int game_getCycles();
    void model_addAnim(int index, const std::string& name, const std::string& filename,
                             std::optional<int> orientation);
    void model_runAnim(int index, const std::string& name, std::optional<int> phase);
    void model_setAnim(int index, const std::string& name, int phase);
    void model_useSpecialAnim(int index, const std::string& name, int phase);
    void model_setEffect(int index, const std::string& name);
    std::pair<int, int> model_getLoc(int index);
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
    void model_setViewShift(int index, int dx, int dy);
    std::pair<int, int> model_getViewShift(int index);
    void model_setBusy(int index, bool busy);
    bool model_isTalking(int index);
    void model_talk(int index, const std::string& name, std::optional<int> volume, std::optional<int> loops, bool dialogFlag);
    void model_killSound(int index);
    bool model_equals(int index, int x, int y);
    void sound_addSound(const std::string& name, const std::string& filename);
    void sound_playSound(const std::string& name, std::optional<int> volume);
    void sound_playMusic(const std::string& filename);
    void sound_stopMusic();
    bool game_isPlanning();
    void game_planAction(LuaCallback function);
    void game_killPlan();
    void game_addDecor(const std::string& type, int m1, int m2, int dx1, int dy1, int dx2, int dy2);
    void game_setScreenShift(float dx, float dy);
    void game_changeBg(const std::string& filename);
    bool dialog_isDialog();
    void dialog_addFont(const std::string& name, int r1, int g1, int b1, std::optional<int> r2, std::optional<int> g2, std::optional<int> b2);
    void dialog_addDialog(const std::string& name, const std::string& lang, const std::string& soundfile,
                                 const std::optional<std::string>& fontname, const std::optional<std::string>& subtitle);
    std::string options_getParam(const std::string& name);

    void setBusy(BusyReason reason, bool busy = true);
    bool isBusy(BusyReason reason) const;
    void clearSchedule();

    constexpr static int index_talk_both = -1;
    constexpr static int index_free_space = -1;
};

#endif //FISH_FILLETS_LEVEL_H