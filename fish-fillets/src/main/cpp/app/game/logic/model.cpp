#include "model.h"

static constexpr auto baseTime = 300ms; // 3 frames at 10 FPS. Swimming animations have 6 frames, but that would be too slow. Only half will be seen unless motion continues in the same direction.
static constexpr float warpIncrement = .2f;

static const std::map<std::string, std::tuple<Model::Type, bool, Model::SupportType, Model::Weight>> modelTypes{{
    {"item_light", {Model::Type::item_light, false, Model::SupportType::none, Model::Weight::light}},
    {"item_heavy", {Model::Type::item_heavy, false, Model::SupportType::none, Model::Weight::heavy}},
    {"item_fixed", {Model::Type::wall, false, Model::SupportType::wall, Model::Weight::none}},
    {"fish_small", {Model::Type::fish_small, true, Model::SupportType::small, Model::Weight::none}},
    {"fish_big", {Model::Type::fish_big, true, Model::SupportType::big, Model::Weight::none}},
    {"fish_old_small", {Model::Type::fish_old_small, true, Model::SupportType::small, Model::Weight::none}},
    {"fish_old_big", {Model::Type::fish_old_big, true, Model::SupportType::big, Model::Weight::none}},
    {"bonus_box", {Model::Type::bonus_box, false, Model::SupportType::none, Model::Weight::light}},
    {"bonus_exit", {Model::Type::bonus_exit, false, Model::SupportType::none, Model::Weight::light}},
    {"virtual", {Model::Type::virt, false, Model::SupportType::none, Model::Weight::none}},
}};

Model::Model(int index, const std::string& type, int x, int y, const std::string& shape) :
        m_index(index),
        m_goal(Goal::none),
        m_position{x, y},
        m_move(),
        m_viewShift(),
        m_viewShiftSpeed(),
        m_shape(shape),
        m_pushing(false),
        m_driven(false),
        m_hidden(false),
        m_action(Action::base),
        m_orientation(Orientation::left),
        m_touchDir(),
        m_warp(1.f),
        m_effect{Effect::none, 0}
{
    std::tie(m_type, m_alive, m_supportType, m_weight) = modelTypes.at(type);
}

void Model::turn() {
    m_orientation = (Orientation)(1 - m_orientation);
}

void Model::displace(ICoords d, bool pushing) {
    if(!pushing && (FCoords(d) || m_delta))
        m_warp += warpIncrement;
    else
        deltaStop();
    m_move = d;
    m_pushing = pushing;
    m_touchDir = {};
    if(!pushing)
        Log::verbose<Log::motion>("falling ", index(), " warp ", m_warp, " delta ", m_delta, " pos ", m_position);
}

bool Model::syncFall(const Model& other) {
    if(m_warp > other.m_warp) {
        Log::verbose<Log::motion>("sync ", index(), " [", m_warp, ",", m_delta, "] / ", other.index(), " [", other.m_warp, ",", other.m_delta, "]", " ", m_position - other.m_position);
        m_warp = other.m_warp;
        if(m_delta > other.m_delta)
           m_delta = other.m_delta;
        return true;
    } else
        return false;
}

void Model::deltaMove(std::chrono::duration<float> dt, float speed) {
    if(m_move && m_action != Action::busy) {
        m_delta += dt / baseTime * speed * m_warp * FCoords(m_move);
        if (m_delta >= m_move) {
            m_position += m_move;
            m_delta -= m_move;
            m_move = {};
        }
    }
}

void Model::instaMove() {
    m_position += m_move;
    m_delta = {};
    m_move = {};
}

void Model::die() {
    m_alive = false;
    m_type = Type::item_light;
    m_supportType = SupportType::weak;
    m_weight = Weight::light;
}

void Model::bonusSwitch(bool value) {
    if(m_type == Type::bonus_box)
        m_supportType = value ? SupportType::wall : SupportType::none;
    if(m_type == Type::bonus_exit)
        m_hidden = value;
}

void Model::disappear(bool moveOut) {
    m_hidden = true;
    if(moveOut)
        m_position = ICoords{-5, 0};
}

void Model::deltaStop() {
    m_delta = {};
    m_warp = 1.f;
}

FCoords Model::fxy() const {
    if(m_move)
        return m_position + m_delta.clamp(1.f) + m_viewShift;
    else
        return m_position + m_viewShift;
}

bool Model::intersects(Model& other, Direction d) const {
    return shape().intersects(other.shape(), other.xy() - (xy() + d));
}

void Model::setEffect(const std::string& name, float startTime) {
    m_effect = {readEffect(name), startTime};
}

Model::Effect Model::readEffect(const std::string& name) {
    if(name == "none")
        return Effect::none;
    else if(name == "disintegrate")
        return Effect::disintegrate;
    else if(name == "invisible")
        return Effect::invisible;
    else if(name == "reverse")
        return Effect::reverse;
    else if(name == "mirror")
        return Effect::mirror;
    else if(name == "zx")
        return Effect::zx;
    else {
        Log::error("Unhandled effect: ", name);
        return Effect::none;
    }
}
