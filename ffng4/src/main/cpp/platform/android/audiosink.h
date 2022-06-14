#ifndef FISH_FILLETS_ANDROID_AUDIO_SINK_H
#define FISH_FILLETS_ANDROID_AUDIO_SINK_H

#include <oboe/Oboe.h>

class AudioSink : public oboe::AudioStreamDataCallback {
    Audio& m_audio;
    oboe::AudioStream* m_stream;

public:
    AudioSink(Audio& iface);
    AudioSink(const AudioSink&) = delete;
    AudioSink& operator=(const AudioSink&) = delete;
    ~AudioSink();

    void start();
    void stop();

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FISH_FILLETS_ANDROID_AUDIO_SINK_H