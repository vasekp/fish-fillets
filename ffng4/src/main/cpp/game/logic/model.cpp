#include "model.h"

std::tuple<Model::Type, bool, Model::SupportType, Model::Weight> decode(const std::string& type) {
    if(type == "item_light")
        return {Model::Type::item_light, false, Model::SupportType::none, Model::Weight::light};
    else if(type == "item_heavy")
        return {Model::Type::item_heavy, false, Model::SupportType::none, Model::Weight::heavy};
    else if(type == "item_fixed")
        return {Model::Type::wall, false, Model::SupportType::wall, Model::Weight::none};
    else if(type == "fish_small")
        return {Model::Type::fish_small, true, Model::SupportType::small, Model::Weight::none};
    else if(type == "fish_big")
        return {Model::Type::fish_big, true, Model::SupportType::big, Model::Weight::none};
    else if(type == "virtual")
        return {Model::Type::virt, false, Model::SupportType::none, Model::Weight::none};
    else
        Log::fatal("Type not implemented: ", type);
}

Model::Model(int index, const std::string& type, int x, int y, const std::string& shape) :
        m_index(index),
        m_goal(Goal::none),
        m_position{x, y},
        m_move(),
        m_viewShift(),
        m_shape(shape),
        m_pushing(false),
        m_driven(false),
        m_action(Action::base),
        m_orientation(Orientation::left),
        m_touchDir(),
        m_warp(1.f),
        m_effect{Effect::none, 0}
{
    std::tie(m_type, m_alive, m_supportType, m_weight) = decode(type);
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
        Log::verbose("falling ", index(), " warp ", m_warp, " delta ", m_delta, " pos ", m_position);
}

bool Model::syncFall(const Model* other) {
    if(m_warp > other->m_warp) {
        Log::verbose("sync ", index(), " [", m_warp, ",", m_delta, "] / ", other->index(), " [", other->m_warp, ",", other->m_delta, "]", " ", m_position - other->m_position);
        m_warp = other->m_warp;
        if(m_delta > other->m_delta)
           m_delta = other->m_delta;
        return true;
    } else
        return false;
}

void Model::deltaMove(float dt, float speed) {
    if(m_move && m_action != Action::busy) {
        m_delta += (falling() ? fallSpeed : baseSpeed) * dt * speed * m_warp * FCoords(m_move);
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

bool Model::intersects(Model* other, ICoords d) const {
    return shape().intersects(other->shape(), other->xy() - (xy() + d));
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
