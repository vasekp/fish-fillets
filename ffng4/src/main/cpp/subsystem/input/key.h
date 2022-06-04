#ifndef FISH_FILLETS_KEY_H
#define FISH_FILLETS_KEY_H

#include "common.h"

enum class Key {
    none,
    up,
    down,
    left,
    right,
    smallUp,
    smallDown,
    smallLeft,
    smallRight,
    bigUp,
    bigDown,
    bigLeft,
    bigRight,
    space,
    save,
    load,
    restart,
    options, // TODO
    exit
};

namespace Input {
    Key AndroidKeymap(unsigned code);
    Key CharKeymap(char code);
    Key toKey(Direction dir);
}

#endif //FISH_FILLETS_KEY_H
