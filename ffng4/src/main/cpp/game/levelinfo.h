#ifndef FISH_FILLETS_LEVELINFO_H
#define FISH_FILLETS_LEVELINFO_H

#include "common.h"

struct LevelInfo {
    std::string filename;
    std::string parent;
    bool solved;

    Coords coords;
};

#endif //FISH_FILLETS_LEVELINFO_H