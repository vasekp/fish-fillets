#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "game/graphics/modelanim.h"
#include "subsystem/audio.h"
#include "shape.h"

class LevelLayout;

class Model {
public:
    enum Orientation { // Need to leave this implicitly convertible to int.
        left = 0,
        right = 1
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
        weak,
        small,
        big,
        wall,
        SIZE
    };

    enum class Fish {
        none,
        small,
        big
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

    enum class Goal {
        none,
        alive,
        escape
    };

private:
    int m_index;
    Type m_type;
    SupportType m_supportType;
    Weight m_weight;
    Goal m_goal;
    ICoords m_position;
    ICoords m_move;
    FCoords m_delta;
    ICoords m_viewShift;
    Shape m_shape;
    bool m_alive;
    bool m_pushing;
    bool m_driven;
    Action m_action;
    Orientation m_orientation;
    ModelAnim m_anim;
    AudioSourceRef m_talk;
    ICoords m_touchDir;
    float m_warp;

public:
    Model(int index, const std::string& type, int x, int y, const std::string& shape);

    friend bool operator==(const Model& a, const Model& b) { return &a == &b; }

    int x() const { return m_position.x; }
    int y() const { return m_position.y; }
    ICoords xy() const { return m_position; }
    FCoords fxy() const;
    float fx() const { return fxy().fx(); }
    float fy() const { return fxy().fy(); }

    Type type() const { return m_type; }
    SupportType supportType() const { return m_supportType; };
    Weight weight() const { return m_weight; };
    Orientation orientation() const { return m_orientation; }
    const Shape& shape() const { return m_shape; }
    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }
    int index() const { return m_index; }

    bool alive() const { return m_alive; }
    bool talking() const { return m_talk && !m_talk->done(); }
    bool isVirtual() const { return m_type == Type::virt; }
    bool movable() const { return !(m_type == Type::fish_small || m_type == Type::fish_big || m_type == Type::wall || m_driven); }
    bool moving() const { return (bool)m_move; }
    bool pushing() const { return m_move && m_pushing; }
    bool falling() const { return !alive() && m_move == Direction::down; }
    auto movingDir() const { return m_move; }
    auto& action() { return m_action; }
    auto action() const { return m_action; }
    auto& touchDir() { return m_touchDir; }
    auto& viewShift() { return m_viewShift; }
    auto& goal() { return m_goal; }
    auto goal() const { return m_goal; }
    auto& driven() { return m_driven; }
    auto driven() const { return m_driven; }

    bool intersects(Model* other, ICoords d) const;

    void turn();
    void displace(ICoords d, bool pushing = false);
    void deltaMove(float dt, float speed = 1.f);
    void deltaStop();
    void die();
    void disappear() { m_type = Type::virt; }
    AudioSourceRef& talk() { return m_talk; }

    constexpr static float baseSpeed = 2.f/.6f; // Ideally such that 6 frames (0.6s) are displayed, but that would be too slow, so take half of that.
    constexpr static float fallSpeed = baseSpeed;
    constexpr static float warpIncrement = .2f;
};

#endif //FISH_FILLETS_MODEL_H