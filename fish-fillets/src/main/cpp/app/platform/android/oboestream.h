#ifndef FF_ANDROID_AUDIO_SINK_H
#define FF_ANDROID_AUDIO_SINK_H

#include <oboe/Oboe.h>

class Audio;

class OboeStream : oboe::AudioStreamDataCallback {
public:
    OboeStream(Audio&, oboe::AudioStreamErrorCallback*);
    OboeStream(const OboeStream&) = delete;
    OboeStream& operator=(const OboeStream&) = delete;
    ~OboeStream();

    void start();
    void stop();

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream* audioStream, void* audioData, int32_t numFrames) override;

private:
    Audio& m_audio;
    oboe::AudioStream* m_stream;
};

#endif //FF_ANDROID_AUDIO_SINK_H
