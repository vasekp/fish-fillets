#include "common.h"
#include "layout.h"
#include "level.h"

LevelLayout::LevelLayout(Level& level, int width, int height) :
        m_level(level),
        m_width(width), m_height(height),
        m_speed(1.f),
        m_models_adapted(m_models_internal)
{ }

int LevelLayout::addModel(const std::string& type, int x, int y, const std::string& shape) {
    auto index = (int)m_models_internal.size();
    m_models_internal.push_back(std::make_unique<Model>(index, type, x, y, shape));
    return index;
}

Model* LevelLayout::getModel(int index) const {
    if(index >= 0 && index < m_models_internal.size())
        return m_models_internal[index].get();
    if(m_virtModels.contains(index))
        return m_virtModels.at(index).get();
    m_virtModels.emplace(index, std::make_unique<Model>(index, "virtual", 0, 0, ""));
    Log::debug("virtual model ", index);
    return m_virtModels.at(index).get();
}

void LevelLayout::addRope(const Model *m1, const Model *m2, ICoords d1, ICoords d2) {
    m_ropes.push_back({m1, m2, d1, d2});
}

std::set<Model*> LevelLayout::intersections(const Model* model, ICoords d) {
    if(model->isVirtual())
        return {};
    std::set<Model*> ret;
    for(auto* other : m_models_adapted) {
        if(*other == *model || ret.contains(other) || other->isVirtual())
            continue;
        if(model->intersects(other, d))
            ret.insert(other);
    }
    return ret;
}

std::set<Model*> LevelLayout::obstacles(const Model* root, ICoords d) {
    std::set<Model*> ret;
    std::deque<const Model*> queue;
    queue.push_back(root);

    while(!queue.empty()) {
        const auto model = queue.front();
        for(auto* other : intersections(model, d)) {
            if(ret.contains(other) || other == root)
                continue;
            ret.insert(other);
            if(other->movable())
                queue.push_back(other);
        }
        queue.pop_front();
    }
    Log::verbose("Obstacles: ", (int)ret.size());
    return ret;
}

void LevelLayout::animate(float dt) {
    for (auto* model: m_models_adapted)
        if (model->moving()) {
            auto d = model->movingDir();
            model->deltaMove(dt, m_speed);
            if (!model->moving())
                m_level.rules().registerMotion(model, d);
        }
}

std::pair<int, int> LevelLayout::borderDepth(const Model* model, ICoords delta) const {
    auto x = model->x() + delta.x, y = model->y() + delta.y,
        width = (int)model->shape().width(),
        height = (int)model->shape().height();
    auto depth1 = std::max(
            std::max(-x, x + width - m_width),
            std::max(-y, y + height - m_height));
    auto depth2 = std::max(
            std::max(-x - width, x - m_width),
            std::max(-y - height, y - m_height));
    return {depth1, depth2};
}
