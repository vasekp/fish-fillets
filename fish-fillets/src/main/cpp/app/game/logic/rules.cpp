#include "level.h"
#include "rules.h"
#include "game/screens/levelinput.h"
#include "game/screens/levelscreen.h"

LevelRules::LevelRules(Level &level, LevelLayout &layout) :
    m_level(level),
    m_layout(layout),
    m_curFish(nullptr),
    m_doomed(false),
    m_vintage(false),
    m_bonusExit(nullptr),
    m_queueFixed(false)
{
    m_small = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_small; });
    m_big = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [](const auto& model) { return model->type() == Model::Type::fish_big; });
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
    if(key == Key::space) {
        if(m_queueFixed) {
            Log::error("ignoring space in fixed input queue");
            return;
        }
        m_keyQueue.clear();
        if(m_level.inGoTo())
            m_level.skipGoTo(false);
    }
    m_keyQueue.push_back(key);
}

void LevelRules::clearQueue() {
    if(!m_queueFixed)
        m_keyQueue.clear();
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
        case Key::special:
            bonusSwitch(!m_vintage);
            break;
        default:
            ;
    }
}

bool LevelRules::switchFish(Model* which) {
    Model* target = which != nullptr ? which : m_curFish == m_small ? m_big : m_small;
    if(target->action() != Model::Action::base || !target->alive() || target->hidden() || target->driven())
        return false;
    setFish(target);
    if(!m_vintage && !m_level.inDemo()) {
        m_curFish->action() = Model::Action::activate;
        m_level.transition(ModelAnim::framesActivate, [unit = m_curFish]() {
            unit->action() = Model::Action::base;
        });
    }
    return true;
}

void LevelRules::bonusSwitch(bool value) {
    std::for_each(m_layout.models().begin(), m_layout.models().end(), [&, value](auto* model) {
            if(auto type = model->type(); type == Model::Type::bonus_box || type == Model::Type::bonus_exit) {
                model->bonusSwitch(value);
                updateDepGraph(model);
                if(type == Model::Type::bonus_exit)
                    m_bonusExit = model;
            }
    });
    m_small = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [type = value ? Model::Type::fish_old_small : Model::Type::fish_small](const auto& model) { return model->type() == type; });
    m_big = *std::find_if(m_layout.models().begin(), m_layout.models().end(), [type = value ? Model::Type::fish_old_big : Model::Type::fish_big](const auto& model) { return model->type() == type; });
    setFish(Model::Fish::small);
    clearQueue();
    m_vintage = value;
}

void LevelRules::moveFish(Model::Fish which, Direction d) {
    setFish(which);
    moveFish(d);
}

void LevelRules::moveFish(Direction d) {
    if(!m_curFish || !m_curFish->alive() || m_curFish->action() != Model::Action::base || m_curFish->driven())
        return;

    if((m_curFish->orientation() == Model::Orientation::right && d.x < 0) ||
       (m_curFish->orientation() == Model::Orientation::left && d.x > 0)) {
        if(!m_vintage) {
            m_curFish->action() = Model::Action::turning;
            m_level.transition(ModelAnim::framesTurn, [&, d]() {
                m_curFish->action() = Model::Action::base;
                m_curFish->turn();
                moveFish(d);
            });
        } else {
            m_curFish->turn();
            moveFish(d);
        }
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
    if(m_layout.borderDir(m_curFish) == d && m_curFish->goal() != Model::Goal::escape)
        return;
    for(const auto* model : obs) {
      Log::debug<Log::motion>(m_layout.borderDir(model), d);
        if(m_layout.borderDir(model) == d && model->goal() != Model::Goal::escape)
            return;
    }

    for(auto* model : obs)
        model->displace(d, true);
    Log::debug<Log::motion>(m_curFish->xy(), " -> ", m_curFish->xy() + d);
    m_curFish->displace(d, !obs.empty());
    m_level.recordMove(dirToChar(d));
    m_level.notifyRound();
}

constexpr static std::array<std::tuple<Direction, char, char>, 4> charDirs_normal{{
    {Direction::up, 'u', 'U'},
    {Direction::down, 'd', 'D'},
    {Direction::left, 'l', 'L'},
    {Direction::right, 'r', 'R'}
}};

constexpr static std::array<std::tuple<Direction, char, char>, 4> charDirs_vintage{{
    {Direction::up, 'n', 'N'},
    {Direction::down, 's', 'S'},
    {Direction::left, 'w', 'W'},
    {Direction::right, 'e', 'E'}
}};

constexpr static std::array<std::pair<char, Key>, 8> dirChars_normal{{
    {'u', Key::smallUp},
    {'d', Key::smallDown},
    {'l', Key::smallLeft},
    {'r', Key::smallRight},
    {'U', Key::bigUp},
    {'D', Key::bigDown},
    {'L', Key::bigLeft},
    {'R', Key::bigRight}
}};

constexpr static std::array<std::pair<char, Key>, 8> dirChars_vintage{{
    {'n', Key::smallUp},
    {'s', Key::smallDown},
    {'w', Key::smallLeft},
    {'e', Key::smallRight},
    {'N', Key::bigUp},
    {'S', Key::bigDown},
    {'W', Key::bigLeft},
    {'E', Key::bigRight}
}};

char LevelRules::dirToChar(Direction d) {
    bool small = m_curFish == m_small;
    for(const auto& [dir, cS, cB] : m_vintage ? charDirs_vintage : charDirs_normal)
        if(d == dir)
            return small ? cS : cB;
    Log::fatal("Invalid direction passed to dirToChar");
}

void LevelRules::enqueue(char c) {
    for(const auto& entry : m_vintage ? dirChars_normal : dirChars_vintage)
        if(c == entry.first) {
            m_vintage = !m_vintage; // Provisional, only for the purpose of enqueue()
            keyInput(Key::special);
            break;
        }
    for(const auto& [ch, key] : m_vintage ? dirChars_vintage : dirChars_normal)
        if(c == ch) {
            keyInput(key);
            return;
        }
    Log::fatal("Invalid direction passed to dirToChar");
}

void LevelRules::enqueue(const std::string& chars, bool fixed) {
    m_queueFixed = fixed;
    auto vintage = m_vintage; // Save value; see enqueue(char)
    for(char c : chars)
        enqueue(c);
    m_vintage = vintage;
}

void LevelRules::enqueue(const std::vector<Direction>& dirs, bool fixed) {
    m_queueFixed = fixed;
    for(auto dir : dirs)
        keyInput(Input::toKey(dir));
}

void LevelRules::registerMotion(Model *model, Direction d) {
    m_motions.emplace_back(model, d);
    updateDepGraph(model);
}

bool LevelRules::steady() {
    return std::none_of(m_layout.models().begin(),  m_layout.models().end(), [](const auto& model) { return model->moving(); }) && !m_level.transitioning();
}

bool LevelRules::ready() {
    return steady() && m_keyQueue.empty();
}

void LevelRules::update() {
    evalFalls();
    evalSteel();
    for (auto [model, d] : m_motions)
        if (!model->moving())
            evalMotion(model, d);
    m_motions.clear();

    if(steady()) {
        if(!m_keyQueue.empty()) {
            processKey(m_keyQueue.front());
            m_keyQueue.pop_front();
            if(m_keyQueue.empty())
                m_queueFixed = false;
        } else if(auto key = m_level.input().poll(); key != Key::none && m_level.accepting() && !m_vintage)
            processKey(key);
    }

    for(auto* model : m_layout.models())
        if(!model->moving())
            model->deltaStop();
}

void LevelRules::evalFalls() {
    std::vector<Model*> falling;
    for(auto* model : m_layout.models()) {
        if (model->hidden())
            continue;
        if (model->movable() && m_support[model].none()) {
            clearQueue();
            if(model->movingDir() != Direction::down)
                m_level.notifyRound();
            model->displace(Direction::down);
            falling.push_back(model);
        }
    }
    for(bool stable = false; !stable; ) {
        stable = true;
        for(auto* model: falling)
            for(auto* other: falling)
                if(other != model && model->intersects(other, Direction::down))
                    stable &= !model->syncFall(other);
    }
}

void LevelRules::evalSteel() {
    for(const auto* model : m_layout.models()) {
        if(model->hidden())
            continue;
        if(auto support = m_support[model]; model->movable() && model->weight() == Model::Weight::heavy
                && support.test(Model::SupportType::small)
                && !support.test(Model::SupportType::big)
                && !support.test(Model::SupportType::wall))
            death(m_small);
    }
}

void LevelRules::evalMotion(Model* model, Direction d) {
    Log::verbose<Log::motion>("stopped ", model->index(), " ", d);
    if(model->action() == Model::Action::willBusy)
        model->action() = Model::Action::busy;
    m_level.notifyRound();
    if(!model->alive() && model->weight() != Model::Weight::none && d != Direction::up) {
        const auto& fullSupport = m_support[model];
        if(fullSupport.test(Model::SupportType::wall)) {
            if(d == Direction::down)
                m_level.screen().playSound(model->weight() == Model::Weight::heavy ? "impact_heavy" : "impact_light", .5f);
        } else if(fullSupport.any()) {
            if(d == Direction::down) {
                for(auto* supp : m_layout.obstacles(model, Direction::down))
                    if(supp->alive()) {
                        Log::verbose<Log::motion>("model ", model->size(), " @ ", model->xy(), " killing ", supp->size(), " @ ", supp->xy());
                        death(supp);
                    }
            } else {
                auto dirSupport = directSupport(model);
                if(fullSupport == dirSupport)
                    for(auto* supp : m_layout.intersections(model, Direction::down))
                        if(supp->alive())
                            death(supp);
            }
        }
    }
    checkEscape(model);
}

void LevelRules::death(Model* unit) {
    if(!unit->alive())
        return;
    unit->die();
    updateDepGraph(unit);
    clearQueue();
    unit->anim().removeExtra();
    if(!m_vintage)
        m_level.setModelEffect(unit, "disintegrate");
    m_level.transition(ModelAnim::framesDeath, [&, unit]() {
        unit->disappear();
        updateDepGraph(unit);
    });
    evalMotion(unit, Direction::down); // see this line's commit's comment
    m_doomed = true;
    m_level.notifyDeath(unit);
    if(unit == m_curFish && !switchFish())
        setFish(Model::Fish::none);
}

void LevelRules::checkEscape(Model* model) {
    if(model->goal() == Model::Goal::escape && model->action() != Model::Action::busy) {
        auto dir = m_layout.borderDir(model);
        auto out = m_layout.isOut(model);
        if((model->alive() && !!dir) || (!model->alive() && out))
            m_level.notifyEscape(model);
        if(!!dir) {
            clearQueue();
            model->driven() = true;
            model->displace(dir);
        }
        if(out || (m_bonusExit && model->intersects(m_bonusExit))) {
            std::erase(m_goals, model);
            if(model == m_curFish)
                if(!switchFish()) {
                    setFish(Model::Fish::none);
                    if(m_goals.empty())
                        m_level.success();
                }
            model->disappear();
        }
    }
}

void LevelRules::buildDepGraph() {
    for(const auto* model : m_layout.models()) {
        if (!model->movable() || model->hidden())
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
        if (!model->movable() || model->hidden())
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
        if(supportModels.contains(other) || other->hidden() || other == model)
            continue;
        supportModels.insert(other);
        if(other->supportType() != Model::SupportType::weak) {
            ret.set(other->supportType());
            if(other->movable()) {
                for(auto[above, below] : m_dependencyGraph)
                    if(above == other)
                        queue.push_back(below);
            }
        } else {
            auto otherSupp = calcSupport(other);
            if(otherSupp.any())
                ret.set(other->supportType());
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

bool LevelRules::isFree(Model* model) const {
    //assert(model == m_small || model == m_big);
    auto [tThis, tOther] = model == m_small
        ? std::pair{Model::SupportType::small, Model::SupportType::big}
        : std::pair{Model::SupportType::big, Model::SupportType::small};
    for(auto *other : m_layout.intersections(model, Direction::up))
        if(auto support = m_support.at(other); other->movable()
                && !support.test(Model::SupportType::wall)
                && support.test(tThis)
                && !support.test(tOther))
            return false;
    return true;
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
