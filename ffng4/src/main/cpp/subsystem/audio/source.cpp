#include <utility>

#include "subsystem/audio.h"

AudioSource::AudioSource(std::string filename, std::size_t num_samples, std::unique_ptr<float[]>&& data) :
        m_name(std::move(filename)),
        m_samplesTotal(num_samples),
        m_sampleIndex(0),
        m_data(std::move(data)),
        m_loop(false),
        m_loopStart(0),
        m_loopEnd(0),
        m_volume(1.f),
        m_dialog(false)
{ }

void AudioSource::mixin(float *output, std::size_t numSamples) const {
    auto countRead = std::min(numSamples, m_samplesTotal - m_sampleIndex);
    for (auto i = 0u; i < countRead; i++)
        output[i] += m_volume * m_data[m_sampleIndex++];
    if(m_loop && m_sampleIndex >= m_loopEnd) {
        LOGD("music loop");
        m_sampleIndex = m_loopStart;
        mixin(output + countRead, numSamples - countRead);
    }
}

bool AudioSource::done() const {
    return m_sampleIndex == m_samplesTotal;
}

void AudioSource::rewind() const {
    m_sampleIndex = 0;
}

void AudioSource::setLoop(std::size_t start, std::size_t end) {
    m_loop = (end != start);
    m_loopStart = start;
    m_loopEnd = std::min(end, m_samplesTotal);
    LOGV("setLoop: %d %d", (int)m_loopStart, (int)m_loopEnd);
}