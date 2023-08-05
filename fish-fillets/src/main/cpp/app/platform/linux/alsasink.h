#ifndef FISH_FILLETS_LINUX_AUDIO_SINK_H
#define FISH_FILLETS_LINUX_AUDIO_SINK_H

#include "subsystem/audio.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class AlsaSink {
    Audio& m_audio;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_pause;
    std::atomic<bool> m_quit;

    constexpr static std::size_t bufSizeTarget = 1024;
    constexpr static std::size_t millisRefresh = 10;

public:
    AlsaSink(Audio& iface);
    AlsaSink(const AlsaSink&) = delete;
    AlsaSink& operator=(const AlsaSink&) = delete;
    ~AlsaSink();

    void pause();
    void resume();
};

#endif //FISH_FILLETS_LINUX_AUDIO_SINK_H
