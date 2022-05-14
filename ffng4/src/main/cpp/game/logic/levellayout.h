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
    std::map<const Model*, Model::SupportType> m_support;
    bool m_stable;

public:
    LevelLayout(Level& level, int width, int height);

    void prepare();
    auto& models() { return m_models; }
    int addModel(const std::string& type, int x, int y, const std::string& shape);
    Model& getModel(int index);

    void moveFish(ICoords d);
    void switchFish();

    void draw(float dt);

private:
    bool animate(float dt);
    void update();
    std::set<Model*> obstacles(const Model& unit, ICoords d);
    void buildSupportMap();

    constexpr static float speed = 3.f; // TODO: warp
};

#endif //FISH_FILLETS_LEVELLAYOUT_H