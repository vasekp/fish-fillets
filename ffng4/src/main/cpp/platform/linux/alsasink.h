#ifndef FISH_FILLETS_LINUX_AUDIO_SINK_H
#define FISH_FILLETS_LINUX_AUDIO_SINK_H

#include "subsystem/audio.h"
#include <thread>
#include <atomic>

class AlsaSink {
    Audio& m_audio;
    std::thread m_thread;
    std::atomic<bool> m_quit;

    constexpr static std::size_t bufSizeTarget = 1024;
    constexpr static std::size_t millisRefresh = 10;

public:
    AlsaSink(Audio& iface);
    AlsaSink(const AlsaSink&) = delete;
    AlsaSink& operator=(const AlsaSink&) = delete;
    ~AlsaSink();
};

#endif //FISH_FILLETS_LINUX_AUDIO_SINK_H
