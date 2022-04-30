#ifndef FISH_FILLETS_AUDIO_H
#define FISH_FILLETS_AUDIO_H

#include "instance.h"
#include "platform/ndk.h"

#include <android_native_app_glue.h>
#include <oboe/Oboe.h>
#include <media/NdkMediaExtractor.h>

class AudioStream : public oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {
    oboe::AudioStream* stream{};
    ndk::Asset asset;
    AMediaExtractor* extractor;
    AMediaCodec *codec;
    std::unique_ptr<float[]> dataF;

public:
    AudioStream(Instance*);
    AudioStream(const AudioStream&) = delete;
    AudioStream& operator=(const AudioStream&) = delete;
    ~AudioStream();

private:
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;
};

#endif //FISH_FILLETS_AUDIO_H