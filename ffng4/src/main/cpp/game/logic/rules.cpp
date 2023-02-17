#include "level.h"
#include "rules.h"
#include "game/screens/levelinput.h"
#include "game/screens/levelscreen.h"

LevelRules::LevelRules(Level &level, LevelLayout &layout) : m_level(level), m_layout(layout) {
    m_small = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_small; });
    m_big = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_big; });
    m_curFish = nullptr;
    m_doomed = false;
    setFish(Model::Fish::small);
    for(auto* model : m_layout.models())
        if(model->goal() != Model::Goal::none)
            m_goals.push_back(model);
    buildDepGraph();
}

void LevelRules::setFish(Model::Fish fish) {
    auto* newFish = m_curFish;
    switch(fish) {
        case Model::Fish::small:
            newFish = m_small;
            break;
        case Model::Fish::big:
            newFish = m_big;
            break;
        case Model::Fish::none:
            newFish = nullptr;
            break;
    }
    if(newFish != m_curFish) {
        m_curFish = newFish;
        m_level.notifyFish(fish);
    }
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

Model* LevelRules::activeFish_model() const {
    return m_curFish;
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

bool LevelRules::switchFish(Model* which) {
    Model* target = which != nullptr ? which : m_curFish == m_small ? m_big : m_small;
    if(target->action() == Model::Action::busy || !target->alive() || m_layout.borderDepth(target).first > 0)
        return false;
    if(target == m_curFish)
        return true;
    setFish(target);
    if(which == nullptr) {
        m_curFish->action() = Model::Action::activate;
        m_level.transition(framesActivate, [unit = m_curFish]() {
            unit->action() = Model::Action::base;
        });
    }
    return true;
}

void LevelRules::moveFish(Model::Fish which, Direction d) {
    setFish(which);
    moveFish(d);
}

void LevelRules::moveFish(Direction d) {
    if(!m_curFish->alive() || m_curFish->action() == Model::Action::busy || m_curFish->driven())
        return;
    if((m_curFish->orientation() == Model::Orientation::right && d.x < 0) ||
       (m_curFish->orientation() == Model::Orientation::left && d.x > 0)) {
        m_curFish->action() = Model::Action::turning;
        m_level.transition(framesTurn, [&, d]() {
            m_curFish->action() = Model::Action::base;
            m_curFish->turn();
            moveFish(d);
        });
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
    Log::verbose(m_curFish->xy(), " -> ", m_curFish->xy() + d);
    m_curFish->displace(d, !obs.empty());
    m_level.recordMove(dirToChar(d));
    m_level.notifyRound();
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
        Log::fatal("Invalid direction passed to dirToChar");
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

    bool ready = std::none_of(m_layout.models().begin(),  m_layout.models().end(), [](const auto& model) { return model->moving(); })
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
    std::vector<Model*> falling;
    for(auto* model : m_layout.models()) {
        if (model->isVirtual())
            continue;
        if (model->movable() && m_support[model].none()) {
            m_keyQueue.clear();
            if(model->movingDir() != Direction::down)
                m_level.notifyRound();
            model->displace(Direction::down);
            falling.push_back(model);
        }
    }
    for(bool stable = false; stable;) {
        stable = true;
        for(auto* model: falling)
            for(auto* other: falling)
                if(other != model && model->intersects(other, Direction::down))
                    stable &= !model->syncFall(other);
    }
}

void LevelRules::evalSteel() {
    for(const auto* model : m_layout.models()) {
        if(model->isVirtual())
            continue;
        if(auto support = m_support[model]; model->movable() && model->weight() == Model::Weight::heavy
                && support.test(Model::SupportType::small)
                && !support.test(Model::SupportType::big)
                && !support.test(Model::SupportType::wall))
            death(m_small);
    }
}

void LevelRules::evalMotion(Model* model, Direction d) {
    Log::verbose("stopped ", model->index(), " ", d);
    m_level.notifyRound();
    if(!model->alive() && model->weight() != Model::Weight::none && d != Direction::up) {
        const auto& fullSupport = m_support[model];
        if(fullSupport.test(Model::SupportType::wall)) {
            if(d == Direction::down)
                m_level.screen().playSound(model->weight() == Model::Weight::heavy ? "impact_heavy" : "impact_light", .5f);
        } else if(fullSupport.any()) {
            if(d == Direction::down) {
                for(auto* supp : m_layout.obstacles(model, Direction::down))
                    if(supp->alive())
                        death(supp);
            } else {
                auto dirSupport = directSupport(model);
                if(fullSupport == dirSupport)
                    for(auto* supp : m_layout.intersections(model, Direction::down))
                        if(supp->alive())
                            death(supp);
            }
        }
    }

    if(model->goal() == Model::Goal::escape) {
        auto depth = m_layout.borderDepth(model);
        if((model->alive() && depth.first == 0) || (!model->alive() && depth.second == 0))
            m_level.notifyEscape(model);
        if(depth.first >= 0 && depth.second < 0) {
            model->driven() = true;
            m_keyQueue.clear();
            m_level.schedule([d, model]() {
                model->displace(d);
                return true;
            }, true);
        }
        if(depth.second >= 0) {
            std::erase(m_goals, model);
            if(model == m_curFish)
                if(!switchFish())
                    setFish(Model::Fish::none);
        }
    }
}

void LevelRules::death(Model* unit) {
    if(!unit->alive())
        return;
    m_level.screen().playSound(unit->supportType() == Model::SupportType::small ? "dead_small" : "dead_big");
    unit->die();
    updateDepGraph(unit);
    m_keyQueue.clear();
    m_level.killModelSound(unit); // TODO kill subtitles
    m_level.killDialogs();
    unit->anim().removeExtra();
    m_level.setModelEffect(unit, "disintegrate");
    m_level.transition(framesDeath, [&, unit]() {
        unit->disappear();
        updateDepGraph(unit);
    });
    m_doomed = true;
    m_level.notifyDeath();
    if(unit == m_curFish && !switchFish()) {
        setFish(Model::Fish::none);
        m_level.transition(framesRestart, [&]() { m_level.restartWhenEmpty(); });
    }
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

const EnumBitset<Model::SupportType>& LevelRules::calcSupport(const Model* model) {
    if(m_support.contains(model))
        return m_support[model];

    std::set<const Model*> supportModels;
    EnumBitset<Model::SupportType> ret;
    std::deque<const Model*> queue;

    for(auto [above, below] : m_dependencyGraph)
        if(above == model)
            queue.push_back(below);
    while(!queue.empty()) {
        const auto* other = queue.front();
        queue.pop_front();
        if(supportModels.contains(other) || other->isVirtual() || other == model)
            continue;
        if(other->supportType() != Model::SupportType::weak) {
            supportModels.insert(other);
            ret.set(other->supportType());
            if(other->movable()) {
                for(auto[above, below] : m_dependencyGraph)
                    if(above == other)
                        queue.push_back(below);
            }
        } else {
            if(other != model && calcSupport(other).any()) {
                supportModels.insert(other);
                ret.set(other->supportType());
            }
        }
    }
    ret.reset(Model::SupportType::none);
    return m_support[model] = ret;
}

EnumBitset<Model::SupportType> LevelRules::directSupport(const Model* model) {
    EnumBitset<Model::SupportType> ret;
    for(auto[above, below] : m_dependencyGraph)
        if(above == model && below->supportType() != Model::SupportType::none)
            ret.set(below->supportType());
    return ret;
}

std::pair<Model*, Model*> LevelRules::bothFish() const {
    return {m_small, m_big};
}

bool LevelRules::solvable() const {
    return !m_doomed;
}

bool LevelRules::solved() const {
    return std::all_of(m_goals.begin(),  m_goals.end(), [](const Model* model) {
        return (model->goal() == Model::Goal::alive && model->alive()) || model->goal() == Model::Goal::none;
    });
}
