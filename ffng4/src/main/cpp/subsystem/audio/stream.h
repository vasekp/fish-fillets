#ifndef FISH_FILLETS_AUDIO_STREAM_H
#define FISH_FILLETS_AUDIO_STREAM_H

#include <oboe/Oboe.h>

class AudioStream : public oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {
    oboe::AudioStream* stream;
    std::vector<std::shared_ptr<AudioSource>> sources;

public:
    AudioStream();
    AudioStream(const AudioStream&) = delete;
    AudioStream& operator=(const AudioStream&) = delete;
    ~AudioStream();

    void addSource(AudioSource&& source) {
        sources.push_back(std::make_shared<AudioSource>(std::move(source)));
    }

private:
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FISH_FILLETS_AUDIO_STREAM_H