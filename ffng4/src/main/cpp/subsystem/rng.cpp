#include "rng.h"

RNG::RNG() : m_gen(std::random_device{}()) { }

std::size_t RNG::randomIndex(std::size_t end) {
    std::uniform_int_distribution<std::size_t> dist(0, end - 1);
    return dist(m_gen);
}