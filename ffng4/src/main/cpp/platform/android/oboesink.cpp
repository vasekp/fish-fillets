#include "oboesink.h"

OboeSink::OboeSink(Audio& iface) : AudioSink(iface), m_stream() {
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

    if (auto result = builder.openStream(&m_stream); result != oboe::Result::OK)
        Log::fatal("Failed to open audio stream. Error: ", convertToText(result));
}

OboeSink::~OboeSink() {
    m_stream->stop();
    m_stream->close();
}

void OboeSink::start() {
    oboe::Result result = m_stream->requestStart();
    if (result != oboe::Result::OK)
        Log::error("Failed to start audio sink. Error: ", convertToText(result));
}

void OboeSink::stop() {
    oboe::Result result = m_stream->requestStop();
    if (result != oboe::Result::OK)
        Log::error("Failed to stop audio sink. Error: ", convertToText(result));
}

oboe::DataCallbackResult
OboeSink::onAudioReady(oboe::AudioStream*, void *audioData, int32_t numFrames) {
    m_audio.mix(reinterpret_cast<float*>(audioData), numFrames);
    return oboe::DataCallbackResult::Continue;
}