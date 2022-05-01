#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/source.h"
#include "audio/stream.h"

class Audio {
    Instance* instance;
    AudioStream stream;

public:
    Audio(Instance* instance_) : instance(instance_), stream() { }

    void addSource(AudioSource&& source) {
        stream.addSource(std::move(source));
    }

    AudioSource loadAudio(const std::string& filename);
};

#endif //FISH_FILLETS_AUDIO_H