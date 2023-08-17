#include "oboestream.h"

OboeStream::OboeStream(Audio& iface, oboe::AudioStreamErrorCallback* callback) : m_audio(iface), m_stream() {
    Log::debug<Log::audio>("Oboe stream: construct");
    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(22050);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Fastest); // Actually sounds much better than ::Best!
    builder.setChannelCount(1);
    builder.setDataCallback(this);
    builder.setErrorCallback(callback);

    if (auto result = builder.openStream(&m_stream); result != oboe::Result::OK)
        Log::fatal("Failed to open audio stream. Error: ", convertToText(result));
}

OboeStream::~OboeStream() {
    Log::debug<Log::audio>("Oboe stream: destruct");
    m_stream->stop();
    m_stream->close();
}

void OboeStream::start() {
    oboe::Result result = m_stream->requestStart();
    if (result != oboe::Result::OK)
        Log::error("Failed to start audio sink. Error: ", convertToText(result));
}

void OboeStream::stop() {
    oboe::Result result = m_stream->requestStop();
    if (result != oboe::Result::OK)
        Log::error("Failed to stop audio sink. Error: ", convertToText(result));
}

oboe::DataCallbackResult
OboeStream::onAudioReady(oboe::AudioStream*, void* audioData, int32_t numFrames) {
    m_audio.mix(reinterpret_cast<float*>(audioData), numFrames);
    return oboe::DataCallbackResult::Continue;
}
