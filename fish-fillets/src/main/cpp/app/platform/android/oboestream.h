#ifndef FF_ANDROID_AUDIO_SINK_H
#define FF_ANDROID_AUDIO_SINK_H

#include "subsystem/audio.h"
#include <oboe/Oboe.h>

class OboeStream : oboe::AudioStreamDataCallback {
    Audio& m_audio;
    oboe::AudioStream* m_stream;

public:
    OboeStream(Audio&, oboe::AudioStreamErrorCallback*);
    OboeStream(const OboeStream&) = delete;
    OboeStream& operator=(const OboeStream&) = delete;
    ~OboeStream();

    void start();
    void stop();

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FF_ANDROID_AUDIO_SINK_H
