#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/audiodata.h"
#include "audio/source.h"
#include "platform/audio.h"
#include "audio/sourcelist.h"

class Audio {
    Instance& m_instance;
    AudioSourceList m_sources;
    std::unique_ptr<AudioSink> m_stream;
    std::map<std::string, AudioData::Ref> m_sounds_preload;

public:
    Audio(Instance& instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    void pause();
    void resume();

    void addSource(const AudioSource::Ref& source);
    void removeSource(const AudioSource::Ref& source);
    void clear();
    void clearExcept(const AudioSource::Ref& source);
    void preload(const std::string& filename);

    AudioData::Ref loadOGG(const std::string& filename) const;
    AudioData::Ref loadSound(const std::string& filename) const;
    AudioSource::Ref loadMusic(const std::string& filename) const;

    bool isDialog() const;
    void mix(float* output, std::size_t numSamples);
};

#endif //FISH_FILLETS_AUDIO_H
