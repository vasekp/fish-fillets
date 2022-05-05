#ifndef FISH_FILLETS_AUDIO_STREAM_H
#define FISH_FILLETS_AUDIO_STREAM_H

#include <oboe/Oboe.h>

class AudioStream {
    oboe::AudioStream* m_stream;

public:
    AudioStream(Audio& iface);
    AudioStream(const AudioStream&) = delete;
    AudioStream& operator=(const AudioStream&) = delete;
    ~AudioStream();
};

#endif //FISH_FILLETS_AUDIO_STREAM_H