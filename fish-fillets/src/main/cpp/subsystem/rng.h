#ifndef FISH_FILLETS_RNG_H
#define FISH_FILLETS_RNG_H

#include <random>

class RNG {
    std::minstd_rand m_gen;

public:
    RNG();

    std::size_t randomIndex(std::size_t end);
    float randomFloat(float min, float max);
};

#endif //FISH_FILLETS_RNG_H