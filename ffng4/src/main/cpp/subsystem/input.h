#ifndef FISH_FILLETS_INPUT_H
#define FISH_FILLETS_INPUT_H

#include "common.h"
#include "subsystem/input/key.h"

class Instance;

class Input {
    Instance& m_instance;
    std::bitset<(std::size_t)Key::SIZE> m_pressed;

public:
    Input(Instance& instance) : m_instance(instance) { }

    bool handleKeyDown(Key key);
    bool handleKeyUp(Key key);
    bool pressed(Key key);

private:
    static unsigned index(Key key);
};

#endif //FISH_FILLETS_INPUT_H