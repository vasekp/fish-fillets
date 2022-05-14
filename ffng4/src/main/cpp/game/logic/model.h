#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "game/graphics/modelanim.h"
#include "subsystem/audio.h"
#include "shape.h"

class Model {
public:
    enum Orientation { // Needs to remain convertible to int
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

    enum class Action {
        base,
        busy,
        turning,
        activate
    };

private:
    Type m_type;
    SupportType m_supportType;
    Weight m_weight;
    ICoords m_position;
    ICoords m_move;
    FCoords m_delta;
    Shape m_shape;
    bool m_alive;
    bool m_falling;
    Action m_action;
    Orientation m_orientation;
    ModelAnim m_anim;
    AudioSource m_talk;
    float m_warp;

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
    Orientation orientation() const { return m_orientation; }
    const Shape& shape() const { return m_shape; }
    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }

    bool alive() const { return m_alive; }
    bool talking() const { return m_talk && !m_talk.done(); }
    bool isVirtual() const { return m_type == Type::virt; }
    bool movable() const { return !(m_type == Type::fish_small || m_type == Type::fish_big || m_type == Type::wall); }
    bool moving() const { return (bool)m_move; }
    bool& falling() { return m_falling; }
    bool falling() const { return m_falling; }
    ICoords movingDir() const { return m_move; }
    Action& action() { return m_action; }
    Action action() const { return m_action; }

    void turn();
    void displace(ICoords d);
    void deltaMove(float dt);
    void deltaStop();
    void die();
    AudioSource& talk() { return m_talk; }

    constexpr static float baseSpeed = 2.f/.6f; // Ideally such that 6 frames (0.6s) are displayed, but that would be too slow, so take half of that.
    constexpr static float fallSpeed = 8.f;
    constexpr static float warpIncrement = .2f;
};

#endif //FISH_FILLETS_MODEL_H