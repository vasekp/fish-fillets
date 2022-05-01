#include "subsystem/audio.h"

#include "platform/ndk.h"

AudioStream::AudioStream(Audio* iface) : _iface(iface), _stream() {
    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(22050);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Medium);
    builder.setChannelCount(1);
    builder.setDataCallback(this);
    //builder.setErrorCallback(this);

    if (auto result = builder.openStream(&_stream); result != oboe::Result::OK)
        ::error("Failed to open stream.", "oboe: %s", convertToText(result));

    oboe::Result result = _stream->requestStart();
    if (result != oboe::Result::OK)
        LOGE("Failed to start audio stream. Error: %s", convertToText(result));
}

AudioStream::~AudioStream() {
    _stream->stop();
    _stream->close();
}

oboe::DataCallbackResult
AudioStream::onAudioReady(oboe::AudioStream*, void *audioData, int32_t numFrames) {
    auto outData = reinterpret_cast<float*>(audioData);
    std::memset(audioData, 0, sizeof(float) * numFrames);
    auto& sources = _iface->sources;
    for(const auto& source : sources)
        source->mixin(outData, numFrames);
    auto newEnd = std::remove_if(sources.begin(), sources.end(),
                                 [](auto& source) { return source->done(); });
    if(newEnd != sources.end()) {
        LOGD("AudioStream: removing %lu sources", std::distance(newEnd, sources.end()));
        sources.erase(newEnd, sources.end());
    }
    return oboe::DataCallbackResult::Continue;
}
