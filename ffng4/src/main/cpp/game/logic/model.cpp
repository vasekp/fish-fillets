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

Model::Model(const std::string& type, int x, int y, const std::string& shape) :
        m_position{x, y},
        m_move(),
        m_lastMove(),
        m_shape(shape),
        m_action(Action::base),
        m_orientation(Orientation::left),
        m_warp(1.f)
{
    std::tie(m_type, m_alive, m_supportType, m_weight) = decode(type);
}

void Model::turn() {
    m_orientation = (Orientation)(1 - m_orientation);
}

void Model::displace(ICoords d) {
    if(FCoords(d) || m_delta)
        m_warp += warpIncrement;
    else
        deltaStop();
    m_move = d;
    m_pushing = false;
}

void Model::push(ICoords d) {
    deltaStop();
    displace(d);
    m_pushing = true;
}

void Model::deltaMove(float dt) {
    if (m_move) {
        m_delta += (falling() ? fallSpeed : baseSpeed) * m_warp * dt * FCoords(m_move);
        if (m_delta >= m_move) {
            m_position += m_move;
            m_delta -= m_move;
            m_lastMove = m_move;
            m_move = {};
        }
    }
}

void Model::die() {
    m_alive = false;
    m_type = Type::item_light;
    m_supportType = SupportType::none;
    m_weight = Weight::light;
}

void Model::deltaStop() {
    m_delta = {};
    m_warp = 1.f;
}

ICoords Model::lastMove_consume() {
    auto ret = m_lastMove;
    m_lastMove = {};
    return ret;
}
