#ifndef FISH_FILLETS_LAYOUT_H
#define FISH_FILLETS_LAYOUT_H

#include "model.h"

class Level;

class LevelLayout {
    Level& m_level;
    int m_width;
    int m_height;
    float m_speed;

    std::vector<std::unique_ptr<Model>> m_models_internal;
    mutable std::map<int, std::unique_ptr<Model>> m_virtModels;

    using models_type = decltype(m_models_internal);;
    struct ModelsIterator : models_type::iterator {
        using value_type = Model*;
        using pointer = Model*;
        using reference = Model*;

        reference operator*() { return models_type::iterator::operator*().get(); }
    };

    struct ModelsAdaptor {
        models_type& m_models;
        ModelsAdaptor(models_type& models) : m_models(models) { }

        ModelsIterator begin() { return {m_models.begin()}; }
        ModelsIterator end() { return {m_models.end()}; }
        ModelsIterator begin() const { return {m_models.begin()}; }
        ModelsIterator end() const { return {m_models.end()}; }
        auto size() { return m_models.size(); }
    } m_models_adapted;

    struct RopeDecor {
        const Model* m1;
        const Model* m2;
        ICoords d1;
        ICoords d2;
    };
    std::vector<RopeDecor> m_ropes;

public:
    LevelLayout(Level& level, int width, int height);

    auto& models() { return m_models_adapted; }
    auto width() const { return m_width; }
    auto height() const { return m_height; }

    int addModel(const std::string& type, int x, int y, const std::string& shape);
    void addRope(const Model* m1, const Model* m2, ICoords d1, ICoords d2);
    Model* getModel(int index) const;
    const std::vector<RopeDecor>& getRopes() const { return m_ropes; };
    std::pair<int, int> borderDepth(const Model* model, ICoords delta = {}) const;
    auto& speed() { return m_speed; }

    std::set<Model*> intersections(const Model* model, ICoords d);
    std::set<Model*> obstacles(const Model* root, ICoords d);

    void animate(float dt);

    constexpr static float speed_normal = 1.f;
    constexpr static float speed_loading = 10.f;
};

#endif //FISH_FILLETS_LAYOUT_H