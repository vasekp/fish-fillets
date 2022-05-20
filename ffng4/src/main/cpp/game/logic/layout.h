#ifndef FISH_FILLETS_LAYOUT_H
#define FISH_FILLETS_LAYOUT_H

#include "model.h"

class Level;

class LevelLayout {
    Level& m_level;
    int m_width;
    int m_height;
    float m_speed;

    std::vector<std::unique_ptr<Model>> m_models;
    std::map<int, std::size_t> m_virtModels;

    struct RopeDecor {
        Model* m1;
        Model* m2;
        ICoords d1;
        ICoords d2;
    };
    std::vector<RopeDecor> m_ropes;

public:
    LevelLayout(Level& level, int width, int height);

    auto& models() { return m_models; }
    int addModel(const std::string& type, int x, int y, const std::string& shape);
    void addRope(Model* m1, Model* m2, ICoords d1, ICoords d2);
    Model* getModel(int index);
    const std::vector<RopeDecor>& getRopes() const { return m_ropes; };
    std::pair<int, int> borderDepth(const Model* model, ICoords delta = {}) const;
    auto& speed() { return m_speed; }

    std::set<Model*> intersections(Model* model, ICoords d);
    std::set<Model*> obstacles(Model* root, ICoords d);

    void animate(float dt);

    constexpr static float speed_normal = 1.f;
    constexpr static float speed_loading = 10.f;
};

#endif //FISH_FILLETS_LAYOUT_H