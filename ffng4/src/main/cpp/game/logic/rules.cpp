#include "level.h"
#include "rules.h"

LevelRules::LevelRules(Level &level, LevelLayout &layout) : m_level(level), m_layout(layout) {
    m_small = std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_small; })->get();
    m_big = std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_big; })->get();
    m_curFish = m_small;
    for(const auto& model : m_layout.models())
        if(model->goal() != Model::Goal::none)
            m_goals.emplace_back(model.get(), model->goal());
    buildDepGraph();
}

void LevelRules::keyInput(Key key) {
    m_keyQueue.push_back(key);
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
        case Key::smallUp:
            m_curFish = m_small;
            moveFish(Direction::up);
            break;
        case Key::smallDown:
            m_curFish = m_small;
            moveFish(Direction::down);
            break;
        case Key::smallLeft:
            m_curFish = m_small;
            moveFish(Direction::left);
            break;
        case Key::smallRight:
            m_curFish = m_small;
            moveFish(Direction::right);
            break;
        case Key::bigUp:
            m_curFish = m_big;
            moveFish(Direction::up);
            break;
        case Key::bigDown:
            m_curFish = m_big;
            moveFish(Direction::down);
            break;
        case Key::bigLeft:
            m_curFish = m_big;
            moveFish(Direction::left);
            break;
        case Key::bigRight:
            m_curFish = m_big;
            moveFish(Direction::right);
            break;
        default:
            ;
    }
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

    if(m_layout.borderDepth(m_curFish, d).first > 0 && m_curFish->goal() != Model::Goal::escape)
        return;
    for(auto* model : obs) {
        if(m_layout.borderDepth(model, d).first > 0 && model->goal() != Model::Goal::escape)
            return;
    }

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

    if(ready) {
        m_level.runScheduled();
        if(!m_keyQueue.empty()) {
            processKey(m_keyQueue.front());
            m_keyQueue.pop_front();
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
            m_keyQueue.clear();
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
    if(!model->alive() && model->weight() != Model::Weight::none && d != Direction::up) {
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

    if(model->goal() == Model::Goal::escape) {
        if(auto depth = m_layout.borderDepth(model); depth.first >= 0 && depth.second < 0)
            m_level.scheduleAction([d, model]() { model->displace(d); return true; });
    }
}

void LevelRules::death(Model* unit) {
    if(!unit->alive())
        return;
    m_level.sound_playSound(unit->supportType() == Model::SupportType::small ? "dead_small" : "dead_big", {});
    unit->die();
    updateDepGraph(unit);
    m_keyQueue.clear();
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
        std::deque<Model*> queue;
        queue.push_back(model.get());

        while(!queue.empty()) {
            auto* other = queue.front();
            queue.pop_front();
            if(supportSet.contains(other) || other->isVirtual())
                continue;
            supportSet.insert(other);
            if(other->movable()) {
                for(auto [above, below] : m_dependencyGraph)
                    if(above == other)
                        queue.push_back(below);
            }
        }
        auto suppType = std::accumulate(supportSet.begin(),  supportSet.end(), Model::SupportType::none, [](Model::SupportType prev, const Model* item) {
            return std::max(prev, item->supportType());
        });
        LOGV("%d supported %d", model->index(), suppType);
        m_support[model.get()] = suppType;
    }
}