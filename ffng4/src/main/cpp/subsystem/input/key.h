#ifndef FISH_FILLETS_KEY_H
#define FISH_FILLETS_KEY_H

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
    exit
};

Key AndroidKeymap(unsigned code);
Key CharKeymap(char code);

#endif //FISH_FILLETS_KEY_H
