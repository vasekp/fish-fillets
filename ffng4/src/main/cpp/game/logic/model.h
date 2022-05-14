#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "game/graphics/modelanim.h"
#include "subsystem/audio.h"
#include "shape.h"
#include "displacement.h"

class Model {
public:
    enum dir {
        left,
        right
    };

    enum class Type {
        small,
        big,
        light,
        heavy,
        wall,
        virt
    };

    Type m_type;
    int m_x;
    int m_y;
    Shape m_shape;
    bool m_alive;
    bool m_busy;
    dir m_direction;
    ModelAnim m_anim;
    AudioSource m_talk;

public:
    Model(const std::string& type, int x, int y, const std::string& shape) :
        m_x(x), m_y(y),
        m_shape(shape),
        m_alive(true),
        m_busy(false),
        m_direction(dir::left)
    {
        if(type == "item_light")
            m_type = Type::light;
        else if(type == "item_heavy")
            m_type = Type::heavy;
        else if(type == "item_fixed")
            m_type = Type::wall;
        else if(type == "fish_small")
            m_type = Type::small;
        else if(type == "fish_big")
            m_type = Type::big;
        else if(type == "virtual")
            m_type = Type::virt;
        /*else if(type == "output_left")
            m_type = Type::virt;*/
        else
            ::error("Type not implemented", "Type %s not implemented", type.c_str());
    }

    Model(const Model&) = delete;
    const Model& operator=(const Model&) = delete;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    friend bool operator==(const Model& a, const Model& b) { return &a == &b; }

    Type type() const { return m_type; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    Displacement xy() const { return {(int)m_x, (int)m_y}; }
    dir direction() const { return m_direction; }
    const Shape& shape() const { return m_shape; }
    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }

    bool isAlive() const { return m_alive; }
    bool isBusy() const { return m_busy; }
    bool isTalking() const { return m_talk && !m_talk.done(); }
    bool isVirtual() const { return m_type == Type::virt; }

    void turn() { m_direction = (dir)(1 - m_direction); }
    void displace(Displacement d) { m_x += d.dx; m_y += d.dy; }
    void setTalk(AudioSource source) { m_talk = std::move(source); }
    void setBusy(bool busy) { m_busy = busy; }
    const AudioSource& talk() const { return m_talk; }
};

#endif //FISH_FILLETS_MODEL_H