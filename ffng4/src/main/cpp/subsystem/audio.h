#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/source.h"
#include "audio/stream.h"
#include "audio/sourcelist.h"

class Audio : public oboe::AudioStreamDataCallback {
    Instance& m_instance;
    AudioSourceList m_sources;
    std::unique_ptr<AudioStream> m_stream;

public:
    Audio(Instance& instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    void pause();
    void resume();

    void addSource(AudioSource& source);
    void removeSource(const std::string& name);
    void clear();

    AudioSource loadSound(const std::string& filename);
    AudioSource loadMusic(const std::string& filename);

private:
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FISH_FILLETS_AUDIO_H