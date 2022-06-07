#ifndef FISH_FILLETS_AUDIO_STREAM_H
#define FISH_FILLETS_AUDIO_STREAM_H

//#include <oboe/Oboe.h> // XXX

class AudioStream {
    //oboe::AudioStream* m_stream; // XXX

public:
    AudioStream(Audio& iface);
    AudioStream(const AudioStream&) = delete;
    AudioStream& operator=(const AudioStream&) = delete;
    ~AudioStream();

    void start();
    void stop();
};

#endif //FISH_FILLETS_AUDIO_STREAM_H
