#ifndef FISH_FILLETS_LIVECLOCK_H
#define FISH_FILLETS_LIVECLOCK_H

class LiveClock {
public:
    using rep = float;
    using period = std::ratio<1>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<LiveClock>;
    constexpr static bool is_steady = true;

    time_point now() noexcept {
        return m_running
            ? m_lastTime + duration(std::chrono::steady_clock::now() - m_lastStart)
            : m_lastTime;
    }

    void pause() {
        m_lastTime += duration(std::chrono::steady_clock::now() - m_lastStart);
        m_running = false;
    }

    void resume() {
        m_lastStart = std::chrono::steady_clock::now();
        m_running = true;
    }

private:
    bool m_running = false;
    time_point m_lastTime = {};
    std::chrono::steady_clock::time_point m_lastStart = {};
};

#endif
