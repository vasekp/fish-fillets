#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"

#include "audio/audiodata.h"
#include "audio/source.h"
#include "audio/stream.h"
#include "audio/sourcelist.h"

class Audio /*: public oboe::AudioStreamDataCallback */ { // XXX
    Instance& m_instance;
    AudioSourceList m_sources;
    std::unique_ptr<AudioStream> m_stream;
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

    AudioData::Ref loadSound(const std::string& filename, bool async = true);
    AudioSource::Ref loadMusic(const std::string& filename, bool async = true);

    bool isDialog() const;

private:
    /*oboe::DataCallbackResult // XXX
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;*/

    friend class AudioPreloader;
};

#endif //FISH_FILLETS_AUDIO_H
