#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "common.h"
#include "instance.h"
#include "audio/audiodata.h"
#include "audio/audiotype.h"
#include "audio/audiosource.h"
#include "audio/audiosourcelist.h"

class Audio {
public:
    Audio(Instance& instance);

    AudioData::Ref loadSound(const std::string& filename) const;
    AudioSource::Ref loadMusic(const std::string& filename, bool repeat = true) const;

    void addSource(const AudioSource::Ref& source);
    void removeSource(const AudioSource::Ref& source);
    void clear();
    void clearExcept(const AudioSource::Ref& source);
    void ping();

    void preload(const std::string& filename);
    bool isDialog() const;

    float getVolume(AudioType type);
    void setVolume(AudioType type, float volume);

    void mix(float* output, std::size_t numSamples);

private:
    Instance& m_instance;
    AudioSourceList m_sources;
    std::map<std::string, AudioData::Ref> m_sounds_preload;
    static_assert(std::atomic<float>::is_always_lock_free);
    std::array<std::atomic<float>, 3> m_volumes;
};

namespace decoders {
    AudioData::Ref ogg(Instance& instance, const std::string& filename);
}

#endif //FISH_FILLETS_AUDIO_H
