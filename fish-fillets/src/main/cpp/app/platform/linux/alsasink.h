#ifndef FISH_FILLETS_LINUX_AUDIO_SINK_H
#define FISH_FILLETS_LINUX_AUDIO_SINK_H

#include "common.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <alsa/asoundlib.h>

class Audio;

class AlsaSink {
public:
    AlsaSink(Audio& iface);
    AlsaSink(const AlsaSink&) = delete;
    AlsaSink& operator=(const AlsaSink&) = delete;
    ~AlsaSink();

    void pause();
    void resume();

private:
    Audio& m_audio;
    snd_pcm_t* m_pcm;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_pause;
    std::atomic<bool> m_quit;
    std::size_t m_bufSize;

    void worker();
    bool tryRestart(int error);

    constexpr static std::size_t bufSizeTarget = 1024;
    constexpr static std::size_t millisRefresh = 10;
};

#endif //FISH_FILLETS_LINUX_AUDIO_SINK_H
