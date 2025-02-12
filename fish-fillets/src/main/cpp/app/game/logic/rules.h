#ifndef FISH_FILLETS_RULES_H
#define FISH_FILLETS_RULES_H

#include "common.h"
#include "subsystem/input/key.h"
#include "utils/enumbitset.h"
#include "model.h"

class Level;
class LevelLayout;

class LevelRules {
public:
    LevelRules(Level& level, LevelLayout& layout);

    void registerMotion(Model& model, Direction d);

    void keyInput(Key key);
    void enqueue(const std::string& chars, bool fixed);
    void enqueue(const std::vector<Direction>& chars, bool fixed);
    void clearQueue();
    void skipLoad(Model::Fish active = Model::Fish::none);
    void update();
    bool ready();
    void checkEscape(Model& model);

    bool switchFish(Model* which = nullptr);
    Model::Fish activeFish() const;
    Model* activeFish_model() const;
    std::pair<Model&, Model&> bothFish() const;
    bool solvable() const;
    bool solved() const;
    bool isFree(Model&) const;
    bool isVintage() const { return m_vintage; }

    void bonusSwitch(bool value);

private:
    Level& m_level;
    LevelLayout& m_layout;

    using ModelRef = std::reference_wrapper<Model>;
    ModelRef m_small;
    ModelRef m_big;
    Model* m_curFish;
    bool m_doomed;
    bool m_vintage;
    Model* m_bonusExit;

    std::vector<ModelRef> m_goals;

    std::deque<Key> m_keyQueue;
    bool m_queueFixed;
    std::vector<std::pair<Model*, Direction>> m_motions;
    std::set<std::pair<const Model*, const Model*>> m_dependencyGraph;
    std::map<const Model*, util::EnumBitset<Model::SupportType>> m_support;

    void enqueue(char c);
    void processKey(Key key);
    void moveFish(Model::Fish which, Direction d);
    void moveFish(Direction d);

    void buildDepGraph();
    void updateDepGraph(const Model& model);
    void buildSupportMap();
    const util::EnumBitset<Model::SupportType>& calcSupport(const Model& model);
    util::EnumBitset<Model::SupportType> directSupport(const Model& model);
    void evalFalls();
    void evalMotion(Model& model, Direction d);
    void evalSteel();
    void death(Model& unit);
    void setFish(Model::Fish fish);
    void setFish(Model& which);
    char dirToChar(Direction d);
    bool steady();
};

#endif //FISH_FILLETS_RULES_H
