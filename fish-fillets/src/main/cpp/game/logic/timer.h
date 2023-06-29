#ifndef FISH_FILLETS_TIMER_H
#define FISH_FILLETS_TIMER_H

#include "common.h"
#include <thread>

class Timer {
    std::thread m_thread;
    std::atomic<bool> m_tick;
    std::atomic<bool> m_stop;
    int m_tickCount;

public:
    Timer();
    ~Timer();

    void start();
    void stop();

    bool ticked();
    int tickCount() const;
    void reset();

private:
    void worker();

    constexpr static std::chrono::duration interval = 100ms;
};

#endif //FISH_FILLETS_TIMER_H
