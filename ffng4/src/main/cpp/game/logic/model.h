#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "game/graphics/modelanim.h"
#include "subsystem/audio.h"
#include "shape.h"
#include "displacement.h"

class Model {
public:
    enum Direction { // Needs to remain convertible to int
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

    enum class SupportType {
        none,
        small,
        big,
        wall
    };

    Type m_type;
    SupportType m_supportType;
    int m_x;
    int m_y;
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

    Type type() const { return m_type; }
    SupportType supportType() const { return m_supportType; };
    int x() const { return m_x; }
    int y() const { return m_y; }
    Displacement xy() const { return {(int)m_x, (int)m_y}; }
    Direction direction() const { return m_direction; }
    const Shape& shape() const { return m_shape; }
    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }

    bool isAlive() const { return m_alive; }
    bool isBusy() const { return m_busy; }
    bool isTalking() const { return m_talk && !m_talk.done(); }
    bool isVirtual() const { return m_type == Type::virt; }
    bool isMovable() const { return !(m_type == Type::small || m_type == Type::big || m_type == Type::wall); }
    void setBusy(bool busy) { m_busy = busy; }

    void turn();
    void displace(Displacement d);
    void setTalk(AudioSource source);
    const AudioSource& getTalk() const { return m_talk; }
};

#endif //FISH_FILLETS_MODEL_H