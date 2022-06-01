#include "subsystem/audio.h"

AudioStream::AudioStream(Audio& iface) : m_stream() {
    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(22050);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Fastest); // Actually sounds much better than ::Best!
    builder.setChannelCount(1);
    builder.setDataCallback(&iface);

    if (auto result = builder.openStream(&m_stream); result != oboe::Result::OK)
        Log::fatal("Failed to open stream. Error: ", convertToText(result));
}

AudioStream::~AudioStream() {
    m_stream->stop();
    m_stream->close();
}

void AudioStream::start() {
    oboe::Result result = m_stream->requestStart();
    if (result != oboe::Result::OK)
        Log::error("Failed to start audio stream. Error: ", convertToText(result));
}

void AudioStream::stop() {
    oboe::Result result = m_stream->requestStop();
    if (result != oboe::Result::OK)
        Log::error("Failed to stop audio stream. Error: ", convertToText(result));
}
