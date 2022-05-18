#include "level.h"
#include "rules.h"

LevelRules::LevelRules(Level &level, LevelLayout &layout) : m_level(level), m_layout(layout) {
    m_small = std::find_if(layout.models().begin(), layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_small; })->get();
    m_big = std::find_if(layout.models().begin(), layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_big; })->get();
    m_curFish = m_small;
    buildDepGraph();
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
    if(std::find_if(obs.begin(), obs.end(), [](Model* model) { return !model->movable(); }) != obs.end()) {
        for(auto* model : obs)
            model->touchDir() = d;
        m_curFish->touchDir() = d;
        return;
    }
    if(m_curFish->supportType() == Model::SupportType::small) {
        if (std::find_if(obs.begin(), obs.end(), [](Model *model) { return model->weight() == Model::Weight::heavy; }) != obs.end())
            return;
    }
    for(auto* model : obs)
        model->displace(d, true);
    m_curFish->displace(d, !obs.empty());
}

void LevelRules::registerMotion(Model *model, Direction d) {
    m_motions.emplace_back(model, d);
    updateDepGraph(model);
}

void LevelRules::update() {
    evalFalls();
    evalSteel();
    for (auto [model, d] : m_motions)
        if (!model->moving())
            evalMotion(model, d);
    m_motions.clear();

    bool ready = !std::any_of(m_layout.models().begin(),  m_layout.models().end(), [](auto& model) { return model->moving(); })
            && !m_level.blocked();
    if(ready)
        m_level.notifyRound();

    if(ready) {
        if(!m_keyQueue.empty()) {
            processKey(m_keyQueue.front());
            m_keyQueue.pop();
        } else if(auto key = m_level.input().pool(); key != Key::none) {
            // TODO only repeat directions
            processKey(key);
        }
    }

    for(auto& model : m_layout.models())
        if(!model->moving())
            model->deltaStop();
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
    LOGV("stopped %d [%d,%d]", model->index(), d.x, d.y);
    if(model->alive())
        return;
    if(model->weight() == Model::Weight::none)
        return;
    if(d == Direction::up)
        return;
    switch(m_support[model]) {
        case Model::SupportType::none:
            break;
        case Model::SupportType::wall:
            if(d == Direction::down)
                m_level.sound_playSound(model->weight() == Model::Weight::heavy ? "impact_heavy" : "impact_light", 50);
            break;
        case Model::SupportType::small:
        case Model::SupportType::big:
            const auto support = (d == Direction::down)
                                 ? m_layout.obstacles(model, Direction::down)
                                 : m_layout.intersections(model, Direction::down);
            for(auto* other : support)
                if(other->alive())
                    death(other);
    }
}

void LevelRules::death(Model* unit) {
    if(!unit->alive())
        return;
    m_level.sound_playSound(unit->supportType() == Model::SupportType::small ? "dead_small" : "dead_big", {});
    unit->die();
    updateDepGraph(unit);
    clearQueue();
    m_level.model_killSound(unit->index());
    m_level.game_killPlan();
    m_level.model_setEffect(unit->index(), "disintegrate");
    m_level.blockFor(15 /* 1.5 seconds */, [&, unit]() {
        unit->disappear();
        updateDepGraph(unit);
    });
}

void LevelRules::buildDepGraph() {
    for(auto& model : m_layout.models()) {
        if (!model->movable() || model->isVirtual())
            continue;
        for (auto *other : m_layout.intersections(model.get(), Direction::down))
            m_dependencyGraph.emplace(model.get(), other);
    }
    buildSupportMap();
}

void LevelRules::updateDepGraph(Model *model) {
    std::erase_if(m_dependencyGraph, [model](const auto& pair) {
        return pair.first == model || pair.second == model;
    });
    for(auto* other : m_layout.intersections(model, Direction::down))
        m_dependencyGraph.emplace(model, other);
    for(auto* other : m_layout.intersections(model, Direction::up))
        m_dependencyGraph.emplace(other, model);
    buildSupportMap();
}

void LevelRules::buildSupportMap() {
    m_support.clear();

    for(const auto& model : m_layout.models()) {
        if (!model->movable() || model->isVirtual())
            continue;

        std::set<Model*> supportSet;
        std::queue<Model*> queue;
        queue.push(model.get());

        while(!queue.empty()) {
            auto* other = queue.front();
            queue.pop();
            if(supportSet.contains(other) || other->isVirtual())
                continue;
            supportSet.insert(other);
            if(other->movable()) {
                for(auto [above, below] : m_dependencyGraph)
                    if(above == other)
                        queue.push(below);
            }
        }
        auto suppType = std::accumulate(supportSet.begin(),  supportSet.end(), Model::SupportType::none, [](Model::SupportType prev, const Model* item) {
            return std::max(prev, item->supportType());
        });
        LOGV("%d supported %d", model->index(), suppType);
        m_support[model.get()] = suppType;
    }
}