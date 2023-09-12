#ifndef FISH_FILLETS_LAYOUT_H
#define FISH_FILLETS_LAYOUT_H

#include "common.h"
#include "shape.h"
#include "model.h"

class Level;

class LevelLayout {
public:
    LevelLayout(Level& level, USize size);

    auto& models() { return m_models; }
    const auto& models() const { return m_models; }
    auto size() const { return m_size; }

    int addModel(const std::string& type, int x, int y, const std::string& shape);
    void addRope(const Model& m1, const Model& m2, ICoords d1, ICoords d2);
    Model& getModel(int index);
    Model* modelAt(ICoords coords);

    struct RopeDecor {
        const Model* m1;
        const Model* m2;
        ICoords d1;
        ICoords d2;
    };

    const std::vector<RopeDecor>& getRopes() const { return m_ropes; };

    std::set<Model*> intersections(const Model& model, ICoords d);
    std::set<Model*> obstacles(const Model& root, ICoords d);
    Direction borderDir(const Model& model) const;
    bool isOut(const Model& model) const;
    std::vector<Direction> findPath(const Model& unit, ICoords target);
    std::vector<Direction> randomPath(const Model& unit, int minDistance);

    void animate(std::chrono::duration<float> dt, float speed = speed_normal);

    static constexpr float speed_normal = 1.f;
    static constexpr float speed_loading = 10.f;
    static constexpr float speed_instant = 0.f;

private:
    Level& m_level;
    USize m_size;

    std::deque<Model> m_models;
    mutable std::map<int, std::unique_ptr<Model>> m_virtModels;
    std::vector<RopeDecor> m_ropes;

    static constexpr unsigned maxDim = Shape::maxSize;
    std::array<std::bitset<maxDim>, maxDim> occupiedBitmap(const Model& unit);
};

#endif //FISH_FILLETS_LAYOUT_H
