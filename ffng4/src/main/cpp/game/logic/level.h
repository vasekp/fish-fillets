#ifndef FISH_FILLETS_LEVEL_H
#define FISH_FILLETS_LEVEL_H

#include "subsystem/script.h"
#include "game/structure/levelrecord.h"
#include "levellayout.h"

#include <functional>

class LevelScreen;

class Level : public ScriptReferrer {
    Instance& m_instance;
    LevelScreen& m_screen;
    const LevelRecord& m_record;
    Script m_script;
    std::deque<DelayedFunction> m_plan;
    std::unique_ptr<LevelLayout> m_layout;
    int m_blockCountdown;
    std::function<void()> m_blockCallback;

    struct Dialog {
        std::string text;
        std::string colors;
        std::string soundFile;
    };
    std::map<std::string, Dialog> m_dialogs;

public:
    Level(Instance& instance, LevelScreen& screen, const LevelRecord& record);

    LevelLayout& layout();

    void init();
    void tick();
    void blockFor(int frames, const std::function<void()>& callback);
    bool blocked();

    void level_createRoom(int width, int height, const std::string& bg);
    int level_getRestartCounter();
    int level_getDepth() const;
    bool level_isNewRound();
    bool level_isSolved();
    void game_setRoomWaves(float amplitude, float period, float speed);
    int game_addModel(const std::string& type, int x, int y, const std::string& shape);
    int game_getCycles();
    void model_addAnim(int index, const std::string& name, const std::string& filename,
                             std::optional<int> orientation);
    void model_runAnim(int index, const std::string& name, std::optional<int> phase);
    void model_setAnim(int index, const std::string& name, int phase);
    void model_useSpecialAnim(int index, const std::string& name, int phase);
    std::pair<int, int> model_getLoc(int index);
    std::string model_getAction(int index);
    std::string model_getState(int index);
    bool model_isAlive(int index);
    bool model_isOut(int index);
    bool model_isLeft(int index);
    unsigned model_getW(int index);
    unsigned model_getH(int index);
    void model_setGoal(int index, const std::string& goal);
    void model_change_turnSide(int index);
    void model_setBusy(int index, bool busy);
    bool model_isTalking(int index);
    void model_talk(int index, const std::string& name, std::optional<int> volume, std::optional<int> loops, bool dialogFlag);
    void model_killSound(int index);
    void model_killSoundImpl(Model&);
    void sound_addSound(const std::string& name, const std::string& filename);
    void sound_playSound(const std::string& name, std::optional<int> volume);
    void sound_playMusic(const std::string& filename);
    void sound_stopMusic();
    bool game_isPlanning();
    void game_planAction(DelayedFunction function);
    void game_killPlan();
    bool dialog_isDialog();
    void dialog_addFont(const std::string& name, int r1, int g1, int b1, std::optional<int> r2, std::optional<int> g2, std::optional<int> b2);
    void dialog_addDialog(const std::string& name, const std::string& lang, const std::string& soundfile,
                                 const std::optional<std::string>& fontname, const std::optional<std::string>& subtitle);
    std::string options_getParam(const std::string& name);
};

#endif //FISH_FILLETS_LEVEL_H