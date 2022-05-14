#include "model.h"

std::tuple<Model::Type, Model::SupportType> decode(const std::string& type) {
    if(type == "item_light")
        return {Model::Type::light, Model::SupportType::none};
    else if(type == "item_heavy")
        return {Model::Type::heavy, Model::SupportType::none};
    else if(type == "item_fixed")
        return {Model::Type::wall, Model::SupportType::wall};
    else if(type == "fish_small")
        return {Model::Type::small, Model::SupportType::small};
    else if(type == "fish_big")
        return {Model::Type::big, Model::SupportType::big};
    else if(type == "virtual")
        return {Model::Type::virt, Model::SupportType::none};
    else
        ::error("Type not implemented", "Type %s not implemented", type.c_str());
}

Model::Model(const std::string& type, int x, int y, const std::string& shape) :
        m_x(x), m_y(y),
        m_shape(shape),
        m_alive(true),
        m_busy(false),
        m_direction(Direction::left)
{
    std::tie(m_type, m_supportType) = decode(type);
}

void Model::turn() {
    m_direction = (Direction)(1 - m_direction);
}

void Model::displace(Displacement d) {
    m_x += d.dx;
    m_y += d.dy;
}

void Model::setTalk(AudioSource source) {
    m_talk = std::move(source);
}
