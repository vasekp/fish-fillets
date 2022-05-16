#ifndef FISH_FILLETS_KEY_H
#define FISH_FILLETS_KEY_H

enum class Key {
    none,
    up,
    down,
    left,
    right,
    space,
    save,
    load,
    restart,
    exit,
    SIZE
};

Key AndroidKeymap(unsigned code);

#endif //FISH_FILLETS_KEY_H
