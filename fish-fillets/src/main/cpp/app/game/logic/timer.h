#ifndef FISH_FILLETS_TIMER_H
#define FISH_FILLETS_TIMER_H

#include "common.h"
#include <thread>

class Timer {
public:
    Timer();
    ~Timer();

    void start();
    void stop();

    bool ticked();
    int tickCount() const;
    void reset();

private:
    std::thread m_thread;
    std::atomic<bool> m_tick;
    std::atomic<bool> m_stop;
    int m_tickCount;

    void worker();

    static constexpr auto interval = 100ms;
};

#endif //FISH_FILLETS_TIMER_H
