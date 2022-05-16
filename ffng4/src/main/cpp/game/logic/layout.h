#ifndef FISH_FILLETS_LAYOUT_H
#define FISH_FILLETS_LAYOUT_H

#include "model.h"

class Level;

class LevelLayout {
    Level& m_level;
    [[maybe_unused]] int m_width;
    [[maybe_unused]] int m_height;

    std::vector<std::unique_ptr<Model>> m_models;
    std::map<int, std::size_t> m_virtModels;

public:
    LevelLayout(Level& level, int width, int height);

    auto& models() { return m_models; }
    int addModel(const std::string& type, int x, int y, const std::string& shape);
    Model& getModel(int index);

    std::set<Model*> intersections(Model* model, ICoords d);
    std::set<Model*> obstacles(Model* root, ICoords d);

    void animate(float dt);
};

#endif //FISH_FILLETS_LAYOUT_H