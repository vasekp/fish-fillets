#include "timer.h"

Timer::Timer() :
    m_tick(false),
    m_stop(false),
    m_tickCount(0)
{ }

Timer::~Timer() {
    stop();
}

void Timer::start() {
    if(!m_thread.joinable())
        m_thread = std::thread([this]() { worker(); });
}

void Timer::stop() {
    if(m_thread.joinable()) {
        m_stop.store(true, std::memory_order::release);
        m_thread.join();
    }
}

bool Timer::ticked() {
    if(m_tick.exchange(false, std::memory_order::acq_rel)) {
        m_tickCount++;
        return true;
    } else
        return false;
}

int Timer::tickCount() const {
    return m_tickCount;
}

void Timer::reset() {
    m_tickCount = 0;
}

void Timer::worker() {
    Log::debug<Log::lifecycle>("timer thread started");
    while(!m_stop.load(std::memory_order::relaxed)) {
        m_tick.store(true, std::memory_order::relaxed);
        std::this_thread::sleep_for(interval);
    }
    m_stop.store(false, std::memory_order::release);
    Log::debug<Log::lifecycle>("timer thread exited");
}
