#ifndef FISH_FILLETS_MODEL_H
#define FISH_FILLETS_MODEL_H

#include "modelanim.h"

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
    dir m_direction;
    ModelAnim m_anim;

public:
    Model(std::string type, unsigned x, unsigned y, std::string shape) :
        m_type(std::move(type)),
        m_x(x), m_y(y),
        m_shape(std::move(shape))
    { }

    unsigned x() const { return m_x; }
    unsigned y() const { return m_y; }
    dir direction() const { return m_direction; }

    ModelAnim& anim() { return m_anim; }
    const ModelAnim& anim() const { return m_anim; }
};

#endif //FISH_FILLETS_MODEL_H