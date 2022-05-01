#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/source.h"
#include "audio/stream.h"

class Audio {
    Instance* instance;
    std::vector<std::shared_ptr<AudioSource>> sources;
    std::unique_ptr<AudioStream> stream;

public:
    Audio(Instance* instance_) : instance(instance_) { }

    void activate();
    void shutdown();

    void addSource(AudioSource&& source);

    AudioSource loadAudio(const std::string& filename);

    friend class AudioStream;
};

#endif //FISH_FILLETS_AUDIO_H