#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "common.h"
#include "shape.h"
#include "game/graphics/modelanim.h"
#include "subsystem/audio/audiosource.h"

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
        fish_old_small,
        fish_old_big,
        bonus_box,
        bonus_exit,
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
        willBusy,
        turning,
        activate
    };

    enum class Goal {
        none,
        alive,
        escape
    };

    enum class Effect {
        none,
        disintegrate,
        invisible,
        reverse,
        mirror,
        zx
    };

    Model(int index, const std::string& type, int x, int y, const std::string& shape);
    Model(const Model&) = delete;
    Model(Model&&) = delete;

    friend bool operator==(const Model& a, const Model& b) { return &a == &b; }

    ICoords xy() const { return m_position; }
    FCoords fxy() const;
    ICoords xyFinal() const { return m_position + m_move; }
    USize size() const { return m_shape.size(); }

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
    bool hidden() const { return m_type == Type::virt || m_hidden; }
    bool movable() const { return (m_supportType == SupportType::none || m_supportType == SupportType::weak) && !m_driven; }
    bool moving() const { return (bool)m_move; }
    bool pushing() const { return m_move && m_pushing; }
    bool falling() const { return !alive() && m_move == Direction::down; }
    auto movingDir() const { return m_move; }
    auto& action() { return m_action; }
    auto action() const { return m_action; }
    auto& touchDir() { return m_touchDir; }
    auto viewShift() { return std::tie(m_viewShift, m_viewShiftSpeed); }
    auto& goal() { return m_goal; }
    auto goal() const { return m_goal; }
    auto& driven() { return m_driven; }
    auto driven() const { return m_driven; }

    bool intersects(Model& other, Direction d = Direction::none) const;

    void turn();
    void displace(ICoords d, bool pushing = false);
    bool syncFall(const Model& other);
    void deltaMove(std::chrono::duration<float> dt, float speed = 1.f);
    void instaMove();
    void deltaStop();
    void die();
    void bonusSwitch(bool value);
    void disappear() { m_hidden = true; }
    AudioSource::Ref& talk() { return m_talk; }
    void setEffect(const std::string& name, float startTime);
    auto effect() const { return m_effect; }

    static Effect readEffect(const std::string& name);

private:
    int m_index;
    Type m_type;
    SupportType m_supportType;
    Weight m_weight;
    Goal m_goal;
    ICoords m_position;
    ICoords m_move;
    FCoords m_delta;
    FCoords m_viewShift;
    FCoords m_viewShiftSpeed;
    Shape m_shape;
    bool m_alive;
    bool m_pushing;
    bool m_driven;
    bool m_hidden;
    Action m_action;
    Orientation m_orientation;
    ModelAnim m_anim;
    AudioSource::Ref m_talk;
    ICoords m_touchDir;
    float m_warp;
    struct {
        Effect name;
        float startTime;
    } m_effect;
};

#endif //FISH_FILLETS_MODEL_H
