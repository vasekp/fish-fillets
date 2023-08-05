#ifndef FISH_FILLETS_LINUX_AUDIO_SINK_H
#define FISH_FILLETS_LINUX_AUDIO_SINK_H

#include "subsystem/audio.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <alsa/asoundlib.h>

class AlsaSink {
    Audio& m_audio;
    snd_pcm_t* m_pcm;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_pause;
    std::atomic<bool> m_quit;
    std::size_t m_bufSize;

    constexpr static std::size_t bufSizeTarget = 1024;
    constexpr static std::size_t millisRefresh = 10;

public:
    AlsaSink(Audio& iface);
    AlsaSink(const AlsaSink&) = delete;
    AlsaSink& operator=(const AlsaSink&) = delete;
    ~AlsaSink();

    void pause();
    void resume();

private:
    void worker();
};

#endif //FISH_FILLETS_LINUX_AUDIO_SINK_H
