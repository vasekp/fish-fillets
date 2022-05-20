#ifndef FISH_FILLETS_RULES_H
#define FISH_FILLETS_RULES_H

#include "subsystem/input.h"

class Model;
class Level;
class LevelLayout;

class LevelRules {
    Level& m_level;
    LevelLayout& m_layout;

    Model* m_small;
    Model* m_big;
    Model* m_curFish;

    std::vector<std::pair<const Model*, Model::Goal>> m_goals;

    std::deque<Key> m_keyQueue;
    std::vector<std::pair<Model*, Direction>> m_motions;
    std::set<std::pair<const Model*, const Model*>> m_dependencyGraph;
    std::map<const Model*, Model::SupportType> m_support;

public:
    LevelRules(Level &level, LevelLayout &layout);

    void registerMotion(Model* model, Direction d);

    void keyInput(Key key);
    void update();

private:
    void processKey(Key key);
    void moveFish(Direction d);
    void switchFish();

    void buildDepGraph();
    void updateDepGraph(const Model* model);
    void buildSupportMap();
    Model::SupportType calcSupport(const Model* model);
    void evalFalls();
    void evalMotion(Model* model, Direction d);
    void evalSteel();
    void death(Model*);
    char dirToChar(Direction d);
};

#endif //FISH_FILLETS_RULES_H