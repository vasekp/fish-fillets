#include "common.h"
#include "levellayout.h"
#include "level.h"

LevelLayout::LevelLayout(Level& level, int width, int height) :
        m_level(level),
        m_width(width), m_height(height),
        m_small(nullptr), m_big(nullptr), m_curFish(nullptr),
        m_moving(false), m_ready(false)
{ }

void LevelLayout::prepare() {
    m_small = std::find_if(m_models.begin(), m_models.end(), [](const auto& model) { return model->type() == Model::Type::fish_small; })->get();
    m_big = std::find_if(m_models.begin(), m_models.end(), [](const auto& model) { return model->type() == Model::Type::fish_big; })->get();
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

void LevelLayout::moveFish(ICoords d) {
    if((m_curFish->orientation() == Model::Orientation::right && d.x < 0) ||
            (m_curFish->orientation() == Model::Orientation::left && d.x > 0)) {
        m_curFish->action() = Model::Action::turning;
        m_level.blockFor(3, [&]() {
            m_curFish->action() = Model::Action::base;
            m_curFish->turn();
        });
        return;
    }
    const auto obs = obstacles(*m_curFish, d);
    if(std::find_if(obs.begin(), obs.end(), [](Model* model) { return !model->movable(); }) != obs.end())
        return;
    if(m_curFish->supportType() == Model::SupportType::small) {
        if (std::find_if(obs.begin(), obs.end(), [](Model *model) { return model->weight() == Model::Weight::heavy; }) != obs.end())
            return;
    }
    for(auto* model : obs)
        model->displace(d);
    if(!obs.empty()) {
        m_curFish->deltaStop();
        m_curFish->pushing() = true;
    }
    m_curFish->displace(d);
}

void LevelLayout::update(float dt) {
    if(m_moving)
        m_moving = animate(dt);

    if(!m_moving) {
        if(m_ready && !m_keyQueue.empty()) {
            processKey(m_keyQueue.front());
            m_keyQueue.pop();
        }
        reeval();
    }
}

bool LevelLayout::animate(float dt) {
    bool moving = false;
    for (auto &model: m_models)
        if(model->moving()) {
            model->deltaMove(dt);
            if(model->moving())
                moving = true;
            if(!model->moving())
                model->pushing() = false;
        }
    return moving;
}

void LevelLayout::keyInput(Key key) {
    if(key == Key::space && m_ready)
        switchFish();
    else
        m_keyQueue.push(key);
}

void LevelLayout::processKey(Key key) {
    switch(key) {
        case Key::up:
            moveFish(Direction::up);
            break;
        case Key::down:
            moveFish(Direction::down);
            break;
        case Key::left:
            moveFish(Direction::left);
            break;
        case Key::right:
            moveFish(Direction::right);
            break;
        case Key::space:
            switchFish();
            break;
        default:
            ;
    }
}

void LevelLayout::reeval() {
    buildSupportMap(); // TODO reuse when possible
    for(auto& model : m_models) {
        if(model->isVirtual())
            continue;
        if(model->movable() && m_support[model.get()] == Model::SupportType::none) {
            clearQueue();
            model->falling() = true;
            model->displace(Direction::down);
        }
        if(model->movable() && m_support[model.get()] == Model::SupportType::small && model->weight() == Model::Weight::heavy) {
            clearQueue();
            m_level.sound_playSound("dead_small", {});
            m_small->die();
        }
        if(!model->moving()) {
            model->deltaStop();
            if(model->falling()) {
                model->falling() = false;
                m_level.sound_playSound(model->weight() == Model::Weight::heavy ? "impact_heavy" : "impact_light", 50);
            }
        }
    }
    m_moving = std::any_of(m_models.begin(),  m_models.end(), [](auto& model) { return model->moving(); });
    m_ready = !std::any_of(m_models.begin(),  m_models.end(), [](auto& model) { return
            model->moving() && !model->alive(); }) &&
              !(m_curFish->movingDir() == Direction::down && std::any_of(m_models.begin(),  m_models.end(), [&](auto& model) { return m_support[model.get()] == m_curFish->supportType(); }));
}

void LevelLayout::clearQueue() {
    decltype(m_keyQueue) empty{};
    m_keyQueue.swap(empty);
}

void LevelLayout::switchFish() {
    if(m_curFish == m_small)
        m_curFish = m_big;
    else
        m_curFish = m_small;
    m_curFish->action() = Model::Action::activate;
    m_level.blockFor(4, [&]() { m_curFish->action() = Model::Action::base; });
}

std::set<Model*> LevelLayout::obstacles(const Model &unit, ICoords d) {
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
                if(other.movable())
                    queue.push(&other);
            }
        }
        queue.pop();
    }
    return ret;
}

void LevelLayout::buildSupportMap() {
    {
        decltype(m_support) empty{};
        m_support.swap(empty);
    }

    std::map<Model*, std::vector<Model*>> dependencies;
    for(const auto& model : m_models) {
        if (!model->movable() || model->isVirtual())
            continue;

        std::vector<Model *> deps;
        for (auto &other: m_models) {
            if (other->isVirtual() || *other == *model)
                continue;
            if (model->shape().intersects(other->shape(), other->xy() - (model->xy() + ICoords::down)))
//                if(!other->isMovingDown())
                    deps.push_back(other.get());
        }
        dependencies[model.get()] = std::move(deps);
    }

    std::map<Model*, std::set<Model*>> supportSet;
    for(const auto& model : m_models) {
        if (!model->movable() || model->isVirtual())
            continue;

        std::set<Model*> suppDeep;
        std::queue<Model*> queue;
        queue.push(model.get());

        while(!queue.empty()) {
            auto* other = queue.front();
            queue.pop();
            if(suppDeep.contains(other) || other->isVirtual())
                continue;
            suppDeep.insert(other);
            if(other->movable()) {
                for(auto* dep : dependencies[other])
                    queue.push(dep);
            }
        }

        std::erase_if(suppDeep, [](Model* other) { return other->movable(); });
        supportSet[model.get()] = std::move(suppDeep);
    }

    for(const auto& model : m_models) {
        const auto& set = supportSet[model.get()];
        m_support[model.get()] = std::accumulate(set.begin(),  set.end(), Model::SupportType::none, [](Model::SupportType prev, const Model* item) {
            return std::max(prev, item->supportType());
        });
    }
}