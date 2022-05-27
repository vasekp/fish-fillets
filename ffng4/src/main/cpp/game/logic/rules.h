#ifndef FISH_FILLETS_RULES_H
#define FISH_FILLETS_RULES_H

#include "subsystem/input.h"
#include "game/logic/model.h"

class Level;
class LevelLayout;

class LevelRules {
    Level& m_level;
    LevelLayout& m_layout;

    Model* m_small;
    Model* m_big;
    Model* m_curFish;
    bool m_doomed;

    std::vector<std::pair<const Model*, Model::Goal>> m_goals;

    std::deque<Key> m_keyQueue;
    std::vector<std::pair<Model*, Direction>> m_motions;
    std::set<std::pair<const Model*, const Model*>> m_dependencyGraph;
    std::map<const Model*, EnumBitset<Model::SupportType>> m_support;

public:
    LevelRules(Level &level, LevelLayout &layout);

    void registerMotion(Model* model, Direction d);

    void keyInput(Key key);
    void update();

    Model::Fish activeFish() const;
    std::pair<Model*, Model*> bothFish() const;
    bool solvable() const;

private:
    void processKey(Key key);
    void moveFish(Model::Fish which, Direction d);
    void moveFish(Direction d);
    bool switchFish();

    void buildDepGraph();
    void updateDepGraph(const Model* model);
    void buildSupportMap();
    const EnumBitset<Model::SupportType>& calcSupport(const Model* model);
    EnumBitset<Model::SupportType> directSupport(const Model* model);
    void evalFalls();
    void evalMotion(Model* model, Direction d);
    void evalSteel();
    void death(Model* unit);
    void setFish(Model::Fish fish);
    void setFish(Model* which);
    char dirToChar(Direction d);

    constexpr static int framesTurn = 3; // bound to the animation
    constexpr static int framesActivate = 4;
    constexpr static int framesDeath = 15; // 1.5 seconds; bound to disintegrate.frag
    constexpr static int framesRestart = 50;
};

#endif //FISH_FILLETS_RULES_H