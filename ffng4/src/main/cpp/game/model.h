#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "modelanim.h"
#include "subsystem/audio.h"

class Model {
public:
    enum dir {
        left,
        right
    };

    [[maybe_unused]] std::string m_type;
    unsigned m_x;
    unsigned m_y;
    [[maybe_unused]] std::string m_shape;
    bool m_alive;
    dir m_direction;
    ModelAnim m_anim;
    AudioSource m_talk;

public:
    Model(std::string type, unsigned x, unsigned y, std::string shape) :
        m_type(std::move(type)),
        m_x(x), m_y(y),
        m_shape(std::move(shape)),
        m_alive(true)
    { }

    unsigned x() const { return m_x; }
    unsigned y() const { return m_y; }
    dir direction() const { return m_direction; }

    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }

    bool isAlive() const { return m_alive; }
    bool isTalking() const { return m_talk && !m_talk.done(); }

    void setTalk(AudioSource source) { m_talk = std::move(source); }
};

#endif //FISH_FILLETS_MODEL_H