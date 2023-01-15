#ifndef FF_ANDROID_AUDIO_SINK_H
#define FF_ANDROID_AUDIO_SINK_H

#include "subsystem/audio.h"
#include <oboe/Oboe.h>

class OboeSink : public AudioSink, oboe::AudioStreamDataCallback {
    oboe::AudioStream* m_stream;

public:
    OboeSink(Audio&);
    OboeSink(const OboeSink&) = delete;
    OboeSink& operator=(const OboeSink&) = delete;
    ~OboeSink();

    void start() override;
    void stop() override;

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FF_ANDROID_AUDIO_SINK_H