#include "level.h"
#include "rules.h"
#include "game/screens/levelinput.h"

LevelRules::LevelRules(Level &level, LevelLayout &layout) : m_level(level), m_layout(layout) {
    m_small = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_small; });
    m_big = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_big; });
    setFish(Model::Fish::small);
    for(const auto* model : m_layout.models())
        if(model->goal() != Model::Goal::none)
            m_goals.emplace_back(model, model->goal());
    buildDepGraph();
}

void LevelRules::setFish(Model::Fish fish) {
    switch(fish) {
        case Model::Fish::small:
            m_curFish = m_small;
            break;
        case Model::Fish::big:
            m_curFish = m_big;
            break;
        case Model::Fish::none:
            m_curFish = nullptr;
            break;
    }
    m_level.notifyFish(fish);
}

void LevelRules::setFish(Model* which) {
    setFish(which == m_small ? Model::Fish::small : Model::Fish::big);
}

Model::Fish LevelRules::activeFish() const {
    if(m_curFish == m_small)
        return Model::Fish::small;
    else if(m_curFish == m_big)
        return Model::Fish::big;
    else
        return Model::Fish::none;
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
            moveFish(Model::Fish::small, Direction::up);
            break;
        case Key::smallDown:
            moveFish(Model::Fish::small, Direction::down);
            break;
        case Key::smallLeft:
            moveFish(Model::Fish::small, Direction::left);
            break;
        case Key::smallRight:
            moveFish(Model::Fish::small, Direction::right);
            break;
        case Key::bigUp:
            moveFish(Model::Fish::big, Direction::up);
            break;
        case Key::bigDown:
            moveFish(Model::Fish::big, Direction::down);
            break;
        case Key::bigLeft:
            moveFish(Model::Fish::big, Direction::left);
            break;
        case Key::bigRight:
            moveFish(Model::Fish::big, Direction::right);
            break;
        default:
            ;
    }
}

void LevelRules::switchFish() {
    Model* target = m_curFish == m_small ? m_big : m_small;
    if(target->action() == Model::Action::busy)
        return;
    setFish(target);
    m_curFish->action() = Model::Action::activate;
    m_level.transition(4, [&]() { m_curFish->action() = Model::Action::base; });
}

void LevelRules::moveFish(Model::Fish which, Direction d) {
    setFish(which);
    moveFish(d);
}

void LevelRules::moveFish(Direction d) {
    if(!m_curFish->alive() || m_curFish->action() == Model::Action::busy)
        return;
    if((m_curFish->orientation() == Model::Orientation::right && d.x < 0) ||
       (m_curFish->orientation() == Model::Orientation::left && d.x > 0)) {
        m_curFish->action() = Model::Action::turning;
        m_level.transition(3, [&, d]() {
            m_curFish->action() = Model::Action::base;
            m_curFish->turn();
        });
        m_level.recordMove(dirToChar(d));
        return;
    }

    const auto obs = m_layout.obstacles(m_curFish, d);

    if(m_layout.borderDepth(m_curFish, d).first > 0 && m_curFish->goal() != Model::Goal::escape)
        return;
    for(const auto* model : obs) {
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
    LOGV("[%d,%d] -> [%d,%d]", m_curFish->x(), m_curFish->y(), m_curFish->x() + d.x, m_curFish->y() + d.y);
    m_curFish->displace(d, !obs.empty());
    m_level.recordMove(dirToChar(d));
}

char LevelRules::dirToChar(Direction d) {
    bool small = m_curFish == m_small;
    if(d == Direction::up)
        return small ? 'u' : 'U';
    else if(d == Direction::down)
        return small ? 'd' : 'D';
    else if(d == Direction::left)
        return small ? 'l' : 'L';
    else if(d == Direction::right)
        return small ? 'r' : 'R';
    else
        ::error("Invalid direction passed to dirToChar");
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

    bool ready = !std::any_of(m_layout.models().begin(),  m_layout.models().end(), [](const auto& model) { return model->moving(); })
            && !m_level.transitioning();

    if(ready) {
        m_level.runScheduled();
        if(!m_keyQueue.empty()) {
            processKey(m_keyQueue.front());
            m_keyQueue.pop_front();
        } else if(auto key = m_level.input().pool(); key != Key::none && m_level.accepting())
            processKey(key);
    }

    for(auto* model : m_layout.models())
        if(!model->moving())
            model->deltaStop();
}

void LevelRules::evalFalls() {
    for(auto* model : m_layout.models()) {
        if (model->isVirtual())
            continue;
        if (auto support = m_support[model]; model->movable() && support == Model::SupportType::none) {
            m_keyQueue.clear();
            model->displace(Direction::down);
        }
    }
}

void LevelRules::evalSteel() {
    for(const auto* model : m_layout.models()) {
        if(model->isVirtual())
            continue;
        if(model->movable() && m_support[model] == Model::SupportType::small && model->weight() == Model::Weight::heavy)
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
                    m_level.playSound(model->weight() == Model::Weight::heavy ? "impact_heavy" : "impact_light", .5f);
                break;
            case Model::SupportType::weak:
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
            m_level.schedule([d, model]() {
                model->displace(d);
                return true;
            });
    }
}

void LevelRules::death(Model* unit) {
    if(!unit->alive())
        return;
    m_level.playSound(unit->supportType() == Model::SupportType::small ? "dead_small" : "dead_big");
    unit->die();
    updateDepGraph(unit);
    m_keyQueue.clear();
    m_level.killModelSound(unit);
    m_level.killDialogs();
    unit->anim().removeExtra();
    m_level.setModelEffect(unit, "disintegrate");
    m_level.transition(15 /* 1.5 seconds */, [&, unit]() {
        unit->disappear();
        updateDepGraph(unit);
    });
}

void LevelRules::buildDepGraph() {
    for(const auto* model : m_layout.models()) {
        if (!model->movable() || model->isVirtual())
            continue;
        for (auto *other : m_layout.intersections(model, Direction::down))
            m_dependencyGraph.emplace(model, other);
    }
    buildSupportMap();
}

void LevelRules::updateDepGraph(const Model* model) {
    std::erase_if(m_dependencyGraph, [model](const auto& pair) {
        return pair.first == model || pair.second == model;
    });
    for(const auto* other : m_layout.intersections(model, Direction::down))
        m_dependencyGraph.emplace(model, other);
    for(const auto* other : m_layout.intersections(model, Direction::up))
        m_dependencyGraph.emplace(other, model);
    buildSupportMap();
}

void LevelRules::buildSupportMap() {
    m_support.clear();

    for(const auto* model : m_layout.models()) {
        if (!model->movable() || model->isVirtual())
            continue;
        calcSupport(model);
    }
}

Model::SupportType LevelRules::calcSupport(const Model* model) {
    if(m_support.contains(model))
        return m_support[model];

    std::set<const Model*> supportSet;
    std::deque<const Model*> queue;

    for(auto[above, below] : m_dependencyGraph)
        if(above == model)
            queue.push_back(below);
    while(!queue.empty()) {
        const auto* other = queue.front();
        queue.pop_front();
        if(supportSet.contains(other) || other->isVirtual() || other == model)
            continue;
        if(other->supportType() != Model::SupportType::weak) {
            supportSet.insert(other);
            if(other->movable()) {
                for(auto[above, below] : m_dependencyGraph)
                    if(above == other)
                        queue.push_back(below);
            }
        } else {
            if(other != model && calcSupport(other) != Model::SupportType::none)
                supportSet.insert(other);
        }
    }
    return m_support[model] = std::accumulate(supportSet.begin(),  supportSet.end(), Model::SupportType::none, [](Model::SupportType prev, const Model* item) {
        return std::max(prev, item->supportType());
    });
}

std::pair<Model*, Model*> LevelRules::bothFish() const {
    return {m_small, m_big};
}
