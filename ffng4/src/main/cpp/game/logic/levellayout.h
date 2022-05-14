#ifndef FISH_FILLETS_LEVELLAYOUT_H
#define FISH_FILLETS_LEVELLAYOUT_H

#include "model.h"

class Level;

class LevelLayout {
    [[maybe_unused]] Level& m_level;
    [[maybe_unused]] int m_width;
    [[maybe_unused]] int m_height;

    std::vector<std::unique_ptr<Model>> m_models;
    std::map<int, std::size_t> m_virtModels;
    Model* m_small;
    Model* m_big;
    Model* m_curFish;
    std::map<const Model*, std::set<Model*>> m_support;

public:
    LevelLayout(Level& level, int width, int height) : m_level(level), m_width(width), m_height(height) { }

    void prepare();
    auto& models() { return m_models; }
    int addModel(const std::string& type, int x, int y, const std::string& shape);
    Model& getModel(int index);

    void moveFish(Displacement d);
    void switchFish();

    void animate(float dt);

private:
    std::set<Model*> obstacles(const Model& unit, Displacement d);
    void buildSupportMap();
};

#endif //FISH_FILLETS_LEVELLAYOUT_H