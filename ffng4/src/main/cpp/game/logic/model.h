#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "game/graphics/modelanim.h"
#include "subsystem/audio.h"
#include "shape.h"

class Model {
public:
    enum Direction { // Needs to remain convertible to int
        left,
        right
    };

    enum class Type {
        fish_small,
        fish_big,
        item_light,
        item_heavy,
        wall,
        virt
    };

    enum class SupportType {
        none,
        small,
        big,
        wall
    };

    enum class Weight {
        none,
        light,
        heavy
    };

    Type m_type;
    SupportType m_supportType;
    Weight m_weight;
    ICoords m_position;
    ICoords m_move;
    FCoords m_delta;
    Shape m_shape;
    bool m_alive;
    bool m_busy;
    Direction m_direction;
    ModelAnim m_anim;
    AudioSource m_talk;

public:
    Model(const std::string& type, int x, int y, const std::string& shape);
    Model(const Model&) = delete;
    const Model& operator=(const Model&) = delete;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    friend bool operator==(const Model& a, const Model& b) { return &a == &b; }

    int x() const { return m_position.x; }
    int y() const { return m_position.y; }
    ICoords xy() const { return m_position; }
    float fx() const { return m_position.x + m_delta.fx(); }
    float fy() const { return m_position.y + m_delta.fy(); }

    Type type() const { return m_type; }
    SupportType supportType() const { return m_supportType; };
    Weight weight() const { return m_weight; };
    Direction direction() const { return m_direction; }
    const Shape& shape() const { return m_shape; }
    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }

    bool isAlive() const { return m_alive; }
    bool isBusy() const { return m_busy; }
    bool isTalking() const { return m_talk && !m_talk.done(); }
    bool isVirtual() const { return m_type == Type::virt; }
    bool isMovable() const { return !(m_type == Type::fish_small || m_type == Type::fish_big || m_type == Type::wall); }
    void setBusy(bool busy) { m_busy = busy; }

    void turn();
    void displace(ICoords d);
    void die();
    void setTalk(AudioSource source) { m_talk = std::move(source); }
    const AudioSource& getTalk() const { return m_talk; }
};

#endif //FISH_FILLETS_MODEL_H