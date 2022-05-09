#include "timer.h"
#include <chrono>

void timer_thread(Timer& timer) {
    LOGD("timer thread started");
    while(!timer.m_stop.load(std::memory_order::relaxed)) {
        LOGV("tick");
        timer.m_tick.store(true, std::memory_order::relaxed);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    timer.m_stop.store(false, std::memory_order::release);
    LOGD("timer thread exited");
}

Timer::Timer() : m_thread(), m_tick(false), m_stop(false), m_tickCount(0) {
}

Timer::~Timer() {
    stop();
}

void Timer::start() {
    if(!m_thread.joinable())
        m_thread = std::thread([&]() { timer_thread(*this); });
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

int Timer::tickCount() {
    return m_tickCount;
}