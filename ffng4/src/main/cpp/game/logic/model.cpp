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
        ::error("Type not implemented", "Type %s not implemented", type.c_str());
}

Model::Model(int index, const std::string& type, int x, int y, const std::string& shape) :
        m_index(index),
        m_goal(Goal::none),
        m_position{x, y},
        m_move(),
        m_viewShift(),
        m_shape(shape),
        m_pushing(false),
        m_action(Action::base),
        m_orientation(Orientation::left),
        m_touchDir(),
        m_warp(1.f)
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
}

void Model::deltaMove(float dt, float speed) {
    if (m_move && m_action != Action::busy) {
        m_delta += (falling() ? fallSpeed : baseSpeed) * dt * speed * m_warp * FCoords(m_move);
        if (m_delta >= m_move) {
            m_position += m_move;
            m_delta -= m_move;
            m_move = {};
        }
    }
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
