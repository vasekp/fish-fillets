#ifndef FISH_FILLETS_RNG_H
#define FISH_FILLETS_RNG_H

#include "common.h"
#include "instance.h"
#include <random>

class RNG {
public:
    RNG();

    std::size_t randomIndex(std::size_t end);
    float randomFloat(float min, float max);

private:
    std::minstd_rand m_gen;
};

#endif //FISH_FILLETS_RNG_H
