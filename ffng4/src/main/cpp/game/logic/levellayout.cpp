#include "common.h"
#include "levellayout.h"

void LevelLayout::prepare() {
    m_small = std::find_if(m_models.begin(), m_models.end(), [](const auto& model) { return model->type() == Model::Type::small; })->get();
    m_big = std::find_if(m_models.begin(), m_models.end(), [](const auto& model) { return model->type() == Model::Type::big; })->get();
    m_curFish = m_small;
}

Model& LevelLayout::getModel(int index) {
    if(index >= 0 && index < m_models.size())
        return *m_models[index];
    if(m_virtModels.contains(index))
        return *m_models[m_virtModels.at(index)];
    std::size_t realIndex = m_models.size();
    m_models.push_back(std::make_unique<Model>("virtual", 0, 0, ""));
    LOGD("virtual model %d", index);
    m_virtModels.insert({index, realIndex});
    return *m_models.back();
}

int LevelLayout::addModel(const std::string& type, int x, int y, const std::string& shape) {
    m_models.push_back(std::make_unique<Model>(type, x, y, shape));
    return (int)m_models.size() - 1;
}

void LevelLayout::moveFish(Displacement d) {
    const auto obs = obstacles(*m_curFish, d);
    if(std::find_if(obs.begin(), obs.end(), [](Model* model) { return !model->isMovable(); }) != obs.end())
        return;
    for(auto* model : obs)
        model->displace(d);
    m_curFish->displace(d);
}

void LevelLayout::switchFish() {
    if(m_curFish == m_small)
        m_curFish = m_big;
    else
        m_curFish = m_small;
}

std::set<Model*> LevelLayout::obstacles(const Model &unit, Displacement d) {
    std::set<Model*> ret;
    std::set<const Model*> visited;
    std::queue<const Model*> queue;
    queue.push(&unit);

    while(!queue.empty()) {
        const auto& model = *queue.front();
        if(visited.contains(&model)) {
            queue.pop();
            continue;
        }
        for(auto &uOther: m_models) {
            auto& other = *uOther;
            if(other == model || other == unit || ret.contains(&other))
                continue;
            if(other.isVirtual())
                continue;
            if (model.shape().intersects(other.shape(), other.xy() - (model.xy() + d))) {
                ret.insert(&other);
                if(other.isMovable())
                    queue.push(&other);
            }
        }
        queue.pop();
    }
    return ret;
}
