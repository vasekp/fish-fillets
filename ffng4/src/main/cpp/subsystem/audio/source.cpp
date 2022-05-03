#include "subsystem/audio.h"

void AudioSource::mixin(float *output, std::size_t numSamples) {
    auto countRead = std::min(numSamples, m_samplesTotal - m_sampleIndex);
    for (auto i = 0u; i < countRead; i++)
        output[i] += m_data[m_sampleIndex++];
    if(m_sampleIndex >= m_loopEnd) {
        LOGD("music loop");
        m_sampleIndex = m_loopStart;
        mixin(output + countRead, numSamples - countRead);
    }
}

bool AudioSource::done() const {
    return m_sampleIndex == m_samplesTotal;
}

void AudioSource::rewind() {
    m_sampleIndex = 0;
}

void AudioSource::setLoop(std::size_t start, std::size_t end) {
    m_loop = true;
    m_loopStart = start;
    m_loopEnd = std::min(end, m_samplesTotal);
    LOGV("setLoop: %ld %ld", m_loopStart, m_loopEnd);
}