#include "common.h"
#include "layout.h"
#include "level.h"

LevelLayout::LevelLayout(Level& level, int width, int height) :
        m_level(level),
        m_width(width), m_height(height)
{ }

int LevelLayout::addModel(const std::string& type, int x, int y, const std::string& shape) {
    auto index = (int)m_models.size();
    m_models.push_back(std::make_unique<Model>(index, type, x, y, shape));
    return index;
}

Model& LevelLayout::getModel(int index) {
    if(index >= 0 && index < m_models.size())
        return *m_models[index];
    if(m_virtModels.contains(index))
        return *m_models[m_virtModels.at(index)];
    std::size_t realIndex = m_models.size();
    m_models.push_back(std::make_unique<Model>(index, "virtual", 0, 0, ""));
    LOGD("virtual model %d", index);
    m_virtModels.insert({index, realIndex});
    return *m_models.back();
}

void LevelLayout::addRope(Model *m1, Model *m2, ICoords d1, ICoords d2) {
    m_ropes.push_back({m1, m2, d1, d2});
}

std::set<Model*> LevelLayout::intersections(Model* model, ICoords d) {
    if(model->isVirtual())
        return {};
    std::set<Model*> ret;
    for(const auto& other: m_models) {
        if(*other == *model || ret.contains(other.get()) || other->isVirtual())
            continue;
        if(model->intersects(other.get(), d))
            ret.insert(other.get());
    }
    return ret;
}

std::set<Model*> LevelLayout::obstacles(Model* root, ICoords d) {
    std::set<Model*> ret;
    std::deque<Model*> queue;
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
    LOGD("Obstacles: %d", (int)ret.size());
    return ret;
}
void LevelLayout::animate(float dt) {
    for (auto &model: m_models)
        if (model->moving()) {
            auto d = model->movingDir();
            model->deltaMove(dt);
            if (!model->moving())
                m_level.rules().registerMotion(model.get(), d);
        }
}

bool LevelLayout::isAtBorder(const Model* model) const {
    return model->x() == 0 || model->y() == 0 || model->x() + model->shape().width() == m_width || model->y() + model->shape().height() == m_height;
}