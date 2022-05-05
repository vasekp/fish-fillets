#include "subsystem/audio.h"

AudioStream::AudioStream(Audio& iface) : m_stream() {
    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(22050);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Medium);
    builder.setChannelCount(1);
    builder.setDataCallback(&iface);

    if (auto result = builder.openStream(&m_stream); result != oboe::Result::OK)
        ::error("Failed to open stream.", "oboe: %s", convertToText(result));

    oboe::Result result = m_stream->requestStart();
    if (result != oboe::Result::OK)
        LOGE("Failed to start audio stream. Error: %s", convertToText(result));
}

AudioStream::~AudioStream() {
    m_stream->stop();
    m_stream->close();
}

