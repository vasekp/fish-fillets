#ifndef FISH_FILLETS_LINUX_AUDIO_SINK_H
#define FISH_FILLETS_LINUX_AUDIO_SINK_H

#include <thread>
#include <atomic>

class AudioSink {
    Audio& m_audio;
    std::thread m_thread;
    std::atomic<bool> m_quit;

    constexpr static std::size_t bufSizeTarget = 1024;
    constexpr static std::size_t millisRefresh = 10;

public:
    AudioSink(Audio& iface);
    AudioSink(const AudioSink&) = delete;
    AudioSink& operator=(const AudioSink&) = delete;
    ~AudioSink();

    void start();
    void stop();
};

#endif //FISH_FILLETS_LINUX_AUDIO_SINK_H
