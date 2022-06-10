#include "common.h"
#include "layout.h"
#include "level.h"

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
    Log::debug("virtual model ", index);
    return m_virtModels.at(index).get();
}

Model* LevelLayout::modelAt(ICoords coords) const {
    auto [x, y] = coords;
    if(x < 0 || x >= width() || y < 0 || y >= height())
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
    if(model->isVirtual())
        return {};
    std::set<Model*> ret;
    for(auto* other : m_models_adapted) {
        if(*other == *model || ret.contains(other) || other->isVirtual())
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
            if(other->movable())
                queue.push_back(other);
        }
        queue.pop_front();
    }
    Log::verbose("Obstacles: ", (int)ret.size());
    return ret;
}

void LevelLayout::animate(float dt, float speed) {
    for (auto* model: m_models_adapted)
        if (model->moving()) {
            auto d = model->movingDir();
            if(speed == speed_instant)
                model->instaMove();
            else
                model->deltaMove(dt, speed);
            if (!model->moving())
                m_level.rules().registerMotion(model, d);
        }
}

std::pair<int, int> LevelLayout::borderDepth(const Model* model, ICoords delta) const {
    auto x = model->x() + delta.x, y = model->y() + delta.y,
        width = (int)model->shape().width(),
        height = (int)model->shape().height();
    auto depth1 = std::max(
            std::max(-x, x + width - m_width),
            std::max(-y, y + height - m_height));
    auto depth2 = std::max(
            std::max(-x - width, x - m_width),
            std::max(-y - height, y - m_height));
    return {depth1, depth2};
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
        Log::fatal("unreachable");
}

std::vector<Direction> LevelLayout::findPath(const Model* unit, ICoords target) {
    if(target.x < 0 || target.x >= width() || target.y < 0 || target.y > height())
        return {};
    Log::debug("path from ", unit->xy(), " to ", target, ":");
    std::array<std::bitset<maxDim>, maxDim> occupied;
    /* Mark all occupied fields */
    for(const auto* model : models()) {
        if(model == unit || model->isVirtual() || borderDepth(model).first > 0)
            continue;
        auto [x, y] = model->xy();
        for(auto dx = 0u; dx < model->shape().width(); dx++)
            for(auto dy = 0u; dy < model->shape().height(); dy++)
                occupied[y + dy][x + dx] = occupied[y + dy][x + dx] | model->shape()[dy][dx];
    }
    /* Also avoid borders */
    for(auto x = 0u; x < width(); x++)
        occupied[0][x] = occupied[height() - 1][x] = true;
    for(auto y = 0u; y < height(); y++)
        occupied[y][0] = occupied[y][width() - 1] = true;
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
    ICoords start = unit->xy();
    if(occupied[start.y][start.x])
        Log::fatal("Unit start field marked as occupied.");
    for(int dx = 0; dx < std::min((int)unitWidth, target.x + 1); dx++)
        for(int dy = 0; dy < std::min((int)unitHeight, target.y + 1); dy++)
            if(occupied[target.y - dy][target.x - dx])
                Log::verbose(target - ICoords{dx, dy}, " occupied");
            else
                goto Found;
    Log::debug("no suitable final position found");
    return {};
Found:
    /* Now's the time to start our breadth-first search. */
    std::map<ICoords, Direction> dirs;
    std::deque<std::pair<ICoords, Direction>> queue;
    for(auto dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        queue.emplace_back(start + dir, dir);
    ICoords end{};
    ICoords accept{(int)unitWidth - 1, (int)unitHeight - 1};
    while(!queue.empty()) {
        auto [coords, dir] = queue.front();
        queue.pop_front();
        if(coords.x < 0 || coords.x >= width() || coords.y < 0 || coords.y >= height() || occupied[coords.y][coords.x]) {
            Log::verbose(coords, " inaccessible");
            continue;
        }
        if(dirs.contains(coords)) {
            Log::verbose(coords, " already done: ", dirs[coords]);
            continue;
        }
        Log::verbose(coords, " <- ", dir);
        dirs[coords] = dir;
        if((target - coords).within({}, accept)) {
            end = coords;
            break;
        }
        for(auto nextDir : nextDirs(dir))
            queue.emplace_back(coords + nextDir, nextDir);
    }
    if(!dirs.contains(end)) {
        Log::debug("path not found");
        return {};
    }
    /* Reconstruct path */
    std::vector<Direction> ret;
    for(auto coords = end; coords != start; coords -= dirs[coords])
        ret.push_back(dirs[coords]);
    std::reverse(ret.begin(),  ret.end());
    for(auto dir : ret)
        Log::debug(dir);
    return ret;
}
