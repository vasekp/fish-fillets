#include "level.h"
#include "rules.h"


LevelRules::LevelRules(Level &level, LevelLayout &layout) : m_level(level), m_layout(layout) {
    m_small = std::find_if(layout.models().begin(), layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_small; })->get();
    m_big = std::find_if(layout.models().begin(), layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_big; })->get();
    m_curFish = m_small;
}

void LevelRules::keyInput(Key key) {
    m_keyQueue.push(key);
}

void LevelRules::processKey(Key key) {
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

void LevelRules::clearQueue() {
    decltype(m_keyQueue) empty{};
    m_keyQueue.swap(empty);
}

void LevelRules::switchFish() {
    if(m_curFish == m_small)
        m_curFish = m_big;
    else
        m_curFish = m_small;
    m_curFish->action() = Model::Action::activate;
    m_level.blockFor(4, [&]() { m_curFish->action() = Model::Action::base; });
}

void LevelRules::moveFish(Direction d) {
    if(!m_curFish->alive())
        return;
    if((m_curFish->orientation() == Model::Orientation::right && d.x < 0) ||
       (m_curFish->orientation() == Model::Orientation::left && d.x > 0)) {
        m_curFish->action() = Model::Action::turning;
        m_level.blockFor(3, [&]() {
            m_curFish->action() = Model::Action::base;
            m_curFish->turn();
        });
        return;
    }
    const auto obs = m_layout.obstacles(m_curFish, d);
    if(std::find_if(obs.begin(), obs.end(), [](Model* model) { return !model->movable(); }) != obs.end())
        return;
    if(m_curFish->supportType() == Model::SupportType::small) {
        if (std::find_if(obs.begin(), obs.end(), [](Model *model) { return model->weight() == Model::Weight::heavy; }) != obs.end())
            return;
    }
    for(auto* model : obs)
        model->displace(d);
    if(!obs.empty())
        m_curFish->push(d);
    else
        m_curFish->displace(d);
}

void LevelRules::registerMotion(Model *model, Direction d) {
    m_motions.emplace_back(model, d);
}

void LevelRules::update() {
    buildSupportMap(); // TODO reuse when possible
    evalFalls();
    evalSteel();
    for (auto [model, d] : m_motions)
        if (!model->moving())
            evalMotion(model, d);
    m_motions.clear();
    for(auto& model : m_layout.models())
        if(!model->moving())
            model->deltaStop();

    bool ready = !std::any_of(m_layout.models().begin(),  m_layout.models().end(), [](auto& model) { return model->moving(); });

    if(ready && !m_keyQueue.empty()) {
        processKey(m_keyQueue.front());
        m_keyQueue.pop();
    }
}

void LevelRules::evalFalls() {
    for(auto& model : m_layout.models()) {
        if (model->isVirtual())
            continue;
        if (model->movable() && m_support[model.get()] == Model::SupportType::none) {
            clearQueue();
            model->displace(Direction::down);
        }
    }
}

void LevelRules::evalSteel() {
    for(auto& model : m_layout.models()) {
        if(model->isVirtual())
            continue;
        if(model->movable() && m_support[model.get()] == Model::SupportType::small && model->weight() == Model::Weight::heavy)
            death(m_small);
    }
}

void LevelRules::evalMotion(Model* model, Direction d) {
    LOGD("stopped %d [%d,%d]", model->index(), d.x, d.y);
    if(model->alive())
        return;
    if(model->weight() == Model::Weight::none)
        return;
    if(d == Direction::up)
        return;
    const auto support = (d == Direction::down)
            ? m_layout.obstacles(model, Direction::down)
            : m_layout.intersections(model, Direction::down);
    std::vector<Model*> units;
    bool hasWall = false;
    for(auto* other : support) {
        switch(other->supportType()) {
            case Model::SupportType::small:
            case Model::SupportType::big:
                units.push_back(other);
                break;
            case Model::SupportType::wall:
                hasWall = true;
            default:
                ;
        }
    }
    if(!hasWall)
        for (auto unit : units)
            death(unit);
    if(d == Direction::down && !support.empty())
        m_level.sound_playSound(model->weight() == Model::Weight::heavy ? "impact_heavy" : "impact_light", 50);
}

void LevelRules::death(Model* unit) {
    if(!unit->alive())
        return;
    m_level.sound_playSound(unit->supportType() == Model::SupportType::small ? "dead_small" : "dead_big", {});
    unit->die();
    clearQueue();
    m_level.model_killSound(unit->index());
    m_level.game_killPlan();
    m_level.model_setEffect(unit->index(), "disintegrate");
    m_level.blockFor(15 /* 1.5 seconds */, [unit]() { unit->disappear(); });
}

void LevelRules::buildSupportMap() {
    m_support.clear();
    std::map<Model*, std::vector<Model*>> dependencies;
    for(const auto& model : m_layout.models()) {
        if (!model->movable() || model->isVirtual())
            continue;

        std::vector<Model *> deps;
        for (auto &other: m_layout.models()) {
            if (other->isVirtual() || *other == *model)
                continue;
            if (model->shape().intersects(other->shape(), other->xy() - (model->xy() + Direction::down)))
//                if(!other->isMovingDown())
                deps.push_back(other.get());
        }
        dependencies[model.get()] = std::move(deps);
    }

    std::map<Model*, std::set<Model*>> supportSet;
    for(const auto& model : m_layout.models()) {
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

    for(const auto& model : m_layout.models()) {
        const auto& set = supportSet[model.get()];
        m_support[model.get()] = std::accumulate(set.begin(),  set.end(), Model::SupportType::none, [](Model::SupportType prev, const Model* item) {
            return std::max(prev, item->supportType());
        });
    }
}