#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/audiodata.h"
#include "audio/sink.h"
#include "audio/source.h"
#include "audio/sourcelist.h"

class Audio {
    Instance& m_instance;
    AudioSourceList m_sources;
    AudioSink* m_stream = nullptr;
    std::map<std::string, AudioData::Ref> m_sounds_preload;

public:
    Audio(Instance& instance);

    void bindSink(AudioSink* sink);
    void unbindSink();

    void pause();
    void resume();

    void addSource(const AudioSource::Ref& source);
    void removeSource(const AudioSource::Ref& source);
    void clear();
    void clearExcept(const AudioSource::Ref& source);
    void preload(const std::string& filename);

    AudioData::Ref loadSound(const std::string& filename) const;
    AudioSource::Ref loadMusic(const std::string& filename) const;

    bool isDialog() const;
    void mix(float* output, std::size_t numSamples);
};

namespace decoders {
    AudioData::Ref ogg(Instance& instance, const std::string& filename);
}

#endif //FISH_FILLETS_AUDIO_H