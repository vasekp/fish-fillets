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

private:
    Type m_type;
    SupportType m_supportType;
    Weight m_weight;
    ICoords m_position;
    ICoords m_move;
    FCoords m_delta;
    Shape m_shape;
    bool m_alive;
    bool m_busy;
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

    bool isAlive() const { return m_alive; }
    bool isBusy() const { return m_busy; }
    bool isTalking() const { return m_talk && !m_talk.done(); }
    bool isVirtual() const { return m_type == Type::virt; }
    bool isMovable() const { return !(m_type == Type::fish_small || m_type == Type::fish_big || m_type == Type::wall); }
    void setBusy(bool busy) { m_busy = busy; }
    bool isMoving() const { return (bool)m_move; }
    ICoords movingDir() const { return m_move; }

    void turn();
    void displace(ICoords d, float initWarp = 1.f);
    void deltaMove(float dt);
    void deltaStop();
    void die();
    void setTalk(AudioSource source) { m_talk = std::move(source); }
    const AudioSource& getTalk() const { return m_talk; }

    constexpr static float baseSpeed = 4.f;
    constexpr static float warpIncrement = .2f;
};

#endif //FISH_FILLETS_MODEL_H