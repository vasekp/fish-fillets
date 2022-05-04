#ifndef FISH_FILLETS_AUDIO_STREAM_H
#define FISH_FILLETS_AUDIO_STREAM_H

#include <oboe/Oboe.h>

class AudioStream : public oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {
    Audio& m_iface;
    oboe::AudioStream* m_stream;

public:
    AudioStream(Audio& iface);
    AudioStream(const AudioStream&) = delete;
    AudioStream& operator=(const AudioStream&) = delete;
    ~AudioStream();

private:
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FISH_FILLETS_AUDIO_STREAM_H