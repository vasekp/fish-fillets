#ifndef FISH_FILLETS_LAYOUT_H
#define FISH_FILLETS_LAYOUT_H

#include "model.h"

class Level;

class LevelLayout {
    Level& m_level;
    int m_width;
    int m_height;

    std::vector<std::unique_ptr<Model>> m_models_internal;
    mutable std::map<int, std::unique_ptr<Model>> m_virtModels;

    using models_type = decltype(m_models_internal);
    template<typename B>
    struct ModelsIterator : B {
        auto operator*() { return B::operator*().get(); }
    };

    struct ModelsAdaptor {
        models_type& m_models;
        ModelsAdaptor(models_type& models) : m_models(models) { }

        ModelsIterator<models_type::iterator> begin() { return {m_models.begin()}; }
        ModelsIterator<models_type::iterator> end() { return {m_models.end()}; }
        ModelsIterator<models_type::const_iterator> begin() const { return {m_models.cbegin()}; }
        ModelsIterator<models_type::const_iterator> end() const { return {m_models.cend()}; }
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
    const auto& models() const { return m_models_adapted; }
    auto width() const { return m_width; }
    auto height() const { return m_height; }

    int addModel(const std::string& type, int x, int y, const std::string& shape);
    void addRope(const Model* m1, const Model* m2, ICoords d1, ICoords d2);
    Model* getModel(int index) const;
    Model* modelAt(ICoords coords) const;
    const std::vector<RopeDecor>& getRopes() const { return m_ropes; };

    std::set<Model*> intersections(const Model* model, ICoords d);
    std::set<Model*> obstacles(const Model* root, ICoords d);
    Direction borderDir(const Model* model) const;
    bool isOut(const Model* model) const;
    std::vector<Direction> findPath(const Model* unit, ICoords target);
    std::vector<Direction> randomPath(const Model* unit, int minDistance);

    void animate(float dt, float speed = speed_normal);

private:
    constexpr static unsigned maxDim = Shape::maxSize;
    std::array<std::bitset<maxDim>, maxDim> occupiedBitmap(const Model* unit);

public:
    constexpr static float speed_normal = 1.f;
    constexpr static float speed_loading = 10.f;
    constexpr static float speed_instant = 0.f;
};

#endif //FISH_FILLETS_LAYOUT_H
