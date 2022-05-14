#include "model.h"

std::tuple<Model::Type, Model::SupportType, Model::Weight> decode(const std::string& type) {
    if(type == "item_light")
        return {Model::Type::item_light, Model::SupportType::none, Model::Weight::light};
    else if(type == "item_heavy")
        return {Model::Type::item_heavy, Model::SupportType::none, Model::Weight::heavy};
    else if(type == "item_fixed")
        return {Model::Type::wall, Model::SupportType::wall, Model::Weight::none};
    else if(type == "fish_small")
        return {Model::Type::fish_small, Model::SupportType::small, Model::Weight::none};
    else if(type == "fish_big")
        return {Model::Type::fish_big, Model::SupportType::big, Model::Weight::none};
    else if(type == "virtual")
        return {Model::Type::virt, Model::SupportType::none, Model::Weight::none};
    else
        ::error("Type not implemented", "Type %s not implemented", type.c_str());
}

Model::Model(const std::string& type, int x, int y, const std::string& shape) :
        m_position{x, y},
        m_shape(shape),
        m_alive(true),
        m_busy(false),
        m_direction(Direction::left)
{
    std::tie(m_type, m_supportType, m_weight) = decode(type);
}

void Model::turn() {
    m_direction = (Direction)(1 - m_direction);
}

void Model::displace(ICoords d) {
    m_position += d;
}

void Model::die() {
    m_alive = false;
    m_type = Type::item_light;
    m_supportType = SupportType::none;
}