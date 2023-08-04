#include "common.h"
#include "layout.h"
#include "level.h"
#include "subsystem/rng.h"

LevelLayout::LevelLayout(Level& level, int width, int height) :
        m_level(level),
        m_width(width), m_height(height),
        m_models_adapted(m_models_internal)
{ }

int LevelLayout::addModel(const std::string& type, int x, int y, const std::string& shape) {
    auto index = (int)m_models_internal.size();
    m_models_internal.push_back(std::make_unique<Model>(index, type, x, y, shape));
    return index;
}

Model* LevelLayout::getModel(int index) const {
    if(index >= 0 && (std::size_t)index < m_models_internal.size())
        return m_models_internal[index].get();
    if(m_virtModels.contains(index))
        return m_virtModels.at(index).get();
    m_virtModels.emplace(index, std::make_unique<Model>(index, "virtual", 0, 0, ""));
    Log::verbose<Log::lua>("virtual model ", index);
    return m_virtModels.at(index).get();
}

Model* LevelLayout::modelAt(ICoords coords) const {
    auto [x, y] = coords;
    if(x < 0 || x >= m_width || y < 0 || y >= m_height)
        return nullptr;
    auto it = std::find_if(models().begin(), models().end(), [coords](const Model* model) {
        return model->shape().covers(coords - model->xy());
    });
    if(it == models().end())
        return nullptr;
    else
        return *it;
}

void LevelLayout::addRope(const Model *m1, const Model *m2, ICoords d1, ICoords d2) {
    m_ropes.push_back({m1, m2, d1, d2});
}

std::set<Model*> LevelLayout::intersections(const Model* model, ICoords d) {
    if(model->hidden())
        return {};
    std::set<Model*> ret;
    for(auto* other : m_models_adapted) {
        if(*other == *model || ret.contains(other) || other->hidden())
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
            if(other->movable() && other->supportType() != Model::SupportType::weak)
                queue.push_back(other);
        }
        queue.pop_front();
    }
    Log::verbose<Log::motion>("Obstacles: ", ret.size());
    return ret;
}

void LevelLayout::animate(float dt, float speed) {
    for (auto* model: m_models_adapted) {
        if (model->moving()) {
            auto d = model->movingDir();
            if(speed == speed_instant)
                model->instaMove();
            else
                model->deltaMove(dt, speed);
            if (!model->moving())
                m_level.rules().registerMotion(model, d);
        }
        if(auto [shift, speed] = model->viewShift(); speed)
            shift += dt * speed;
    }
}

Direction LevelLayout::borderDir(const Model* model) const {
    auto [x, y] = model->xy();
    auto [width, height] = model->size();
    if(isOut(model))
        return Direction::none;
    else if(x <= 0)
        return Direction::left;
    else if(x + width >= m_width)
        return Direction::right;
    else if(y <= 0)
        return Direction::up;
    else if(y + height >= m_height)
        return Direction::down;
    else
        return Direction::none;
}

bool LevelLayout::isOut(const Model* model) const {
    auto [x, y] = model->xy();
    auto [width, height] = model->size();
    return x + width <= 0 || y + height <= 0 || x >= m_width || y >= m_height;
}

static std::array<Direction, 3> nextDirs(Direction dir) {
    if(dir == Direction::up)
        return {Direction::up, Direction::left, Direction::right};
    else if(dir == Direction::down)
        return {Direction::down, Direction::left, Direction::right};
    else if(dir == Direction::left)
        return {Direction::left, Direction::up, Direction::down};
    else if(dir == Direction::right)
        return {Direction::right, Direction::up, Direction::down};
    else
        std::unreachable();
}

std::array<std::bitset<LevelLayout::maxDim>, LevelLayout::maxDim> LevelLayout::occupiedBitmap(const Model* unit) {
    std::array<std::bitset<maxDim>, maxDim> occupied;
    /* Mark all occupied fields */
    for(const auto* model : models()) {
        if(model == unit || model->hidden() || model->driven())
            continue;
        auto [x, y] = model->xy();
        auto width = model->shape().width(),
             height = model->shape().height();
        for(auto dx = 0u; dx < width; dx++)
            for(auto dy = 0u; dy < height; dy++)
                occupied[y + dy][x + dx] = occupied[y + dy][x + dx] || model->shape()[dy][dx];
    }
    /* Also avoid borders */
    if(unit->goal() == Model::Goal::escape) {
      for(auto x = 0; x < m_width; x++)
          occupied[0][x] = occupied[m_height - 1][x] = true;
      for(auto y = 0; y < m_height; y++)
          occupied[y][0] = occupied[y][m_width - 1] = true;
    } else {
      for(auto x = 0; x < m_width; x++)
          occupied[m_height][x] = true;
      for(auto y = 0; y < m_height; y++)
          occupied[y][m_width] = true;
    }
    /* Extend occupied fields to the left and above, as unit covers [x, x+unitWidth) × [y, y + unitHeight)
     * and thus can't start where it could clash with an occupied field due to its nonunit size */
    auto unitWidth = unit->shape().width();
    auto unitHeight = unit->shape().height();
    for(auto i = 0u; i < unitWidth - 1; i++)
        for(auto y = 0u; y < maxDim; y++)
            occupied[y] |= occupied[y] >> 1;
    for(auto i = 0u; i < unitHeight - 1; i++)
        for(auto y = 0u; y < maxDim - 1; y++)
            occupied[y] |= occupied[y + 1];
    return occupied;
}

std::vector<Direction> LevelLayout::findPath(const Model* unit, ICoords target) {
    if(target.x < 0 || target.x >= m_width || target.y < 0 || target.y > m_height)
        return {};
    ICoords start = unit->xyFinal();
    Log::debug<Log::gotos>("path from ", start, " to ", target, ":");
    auto occupied = occupiedBitmap(unit);
    /* Check validity */
    if(occupied[start.y][start.x]) {
        Log::error("Unit start field marked as occupied.");
        return {};
    }
    auto unitWidth = unit->shape().width();
    auto unitHeight = unit->shape().height();
    bool found = false;
    for(int dx = 0; !found && dx < std::min((int)unitWidth, target.x + 1); dx++)
        for(int dy = 0; !found && dy < std::min((int)unitHeight, target.y + 1); dy++)
            if(occupied[target.y - dy][target.x - dx])
                Log::verbose<Log::gotos>(target - ICoords{dx, dy}, " occupied");
            else
                found = true;
    if(!found) {
        Log::debug<Log::gotos>("no suitable final position found");
        return {};
    }
    /* Now's the time to start our breadth-first search. */
    std::map<ICoords, Direction> dirs;
    std::deque<std::pair<ICoords, Direction>> queue;
    for(auto dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        queue.emplace_back(start + dir, dir);
    ICoords end{};
    ICoords accept{unitWidth - 1, unitHeight - 1};
    while(!queue.empty()) {
        auto [coords, dir] = queue.front();
        queue.pop_front();
        if(coords.x < 0 || coords.x >= m_width || coords.y < 0 || coords.y >= m_height || occupied[coords.y][coords.x]) {
            Log::verbose<Log::gotos>(coords, " inaccessible");
            continue;
        }
        if(dirs.contains(coords)) {
            Log::verbose<Log::gotos>(coords, " already done: ", dirs[coords]);
            continue;
        }
        Log::verbose<Log::gotos>(coords, " <- ", dir);
        dirs[coords] = dir;
        if((target - coords).within({}, accept)) {
            end = coords;
            break;
        }
        for(auto nextDir : nextDirs(dir))
            queue.emplace_back(coords + nextDir, nextDir);
    }
    if(!dirs.contains(end)) {
        Log::debug<Log::gotos>("path not found");
        return {};
    }
    /* Reconstruct path */
    std::vector<Direction> ret;
    for(auto coords = end; coords != start; coords -= dirs[coords])
        ret.push_back(dirs[coords]);
    Log::debug<Log::gotos>("found (", ret.size(), " moves)");
    std::reverse(ret.begin(),  ret.end());
    return ret;
}

std::vector<Direction> LevelLayout::randomPath(const Model* unit, int minDistance) {
    ICoords start = unit->xyFinal();
    Log::debug<Log::gotos>("path from ", start, " minDistance ", minDistance, ":");
    auto occupied = occupiedBitmap(unit);
    if(occupied[start.y][start.x]) {
        Log::error("Unit start field marked as occupied.");
        return {};
    }
    /* This algorithm is the same as in findPath except that we cover the entire field and also keep distances. */
    std::map<ICoords, std::pair<Direction, int>> dirs;
    std::deque<std::pair<ICoords, std::pair<Direction, int>>> queue;
    for(auto dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        queue.push_back({start + dir, {dir, 1}});
    while(!queue.empty()) {
        auto [coords, pair] = queue.front();
        auto [dir, dist] = pair;
        queue.pop_front();
        if(coords.x < 0 || coords.x >= m_width || coords.y < 0 || coords.y >= m_height || occupied[coords.y][coords.x]) {
            Log::verbose<Log::gotos>(coords, " inaccessible");
            continue;
        }
        if(dirs.contains(coords)) {
            Log::verbose<Log::gotos>(coords, " already done: ", dirs[coords].first, " (", dirs[coords].second, ")");
            continue;
        }
        Log::verbose<Log::gotos>(coords, " <- ", dir, " (", dist, ")");
        dirs[coords] = {dir, dist};
        for(auto nextDir : nextDirs(dir))
            queue.push_back({coords + nextDir, {nextDir, dist + 1}});
    }
    auto maxIt = std::max_element(dirs.begin(), dirs.end(),
        [](const auto& a, const auto& b) {
            return a.second.second < b.second.second;
    });
    /* Filter on minDistance */
    auto maxDist = maxIt == dirs.end() ? 0 : maxIt->second.second;
    Log::debug<Log::gotos>("Max distance: ", maxDist);
    /*std::remove_if(dirs.begin(), dirs.end(),
        [minDistance](const auto& entry) { return entry.second.second < minDistance; });*/
    std::vector<ICoords> pool{};
    /*std::transform(dirs.begin(), dirs.end(), std::back_inserter(pool),
            [](const auto& entry) { return entry.first; });*/
    for(const auto& [coords, entry] : dirs)
        if(entry.second >= minDistance)
            pool.push_back(coords);
    if(pool.empty()) {
        Log::debug<Log::gotos>("No long enough path.");
        return {};
    }
    auto end = pool[m_level.instance().rng().randomIndex(pool.size())];
    /* Reconstruct path */
    std::vector<Direction> ret;
    for(auto coords = end; coords != start; coords -= dirs[coords].first)
        ret.push_back(dirs[coords].first);
    Log::debug<Log::gotos>("target ", end, " (", ret.size(), " moves)");
    std::reverse(ret.begin(),  ret.end());
    return ret;
}
