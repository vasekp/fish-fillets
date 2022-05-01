#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/source.h"
#include "audio/stream.h"

class Audio {
    Instance* m_instance;
    std::vector<std::shared_ptr<AudioSource>> m_sources;
    std::unique_ptr<AudioStream> m_stream;

public:
    Audio(Instance* instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    void addSource(AudioSource&& source);

    AudioSource loadAudio(const std::string& filename);

    friend class AudioStream;
};

#endif //FISH_FILLETS_AUDIO_H