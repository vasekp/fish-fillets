#ifndef FISH_FILLETS_TIMER_H
#define FISH_FILLETS_TIMER_H

#include "common.h"
#include <thread>
#include <atomic>

class Timer {
    std::thread m_thread;
    std::atomic<bool> m_tick;
    std::atomic<bool> m_stop;
    int m_tickCount;

public:
    Timer();
    ~Timer();

    bool ticked();
    int tickCount();

    friend void timer_thread(Timer&);
};

#endif //FISH_FILLETS_TIMER_H