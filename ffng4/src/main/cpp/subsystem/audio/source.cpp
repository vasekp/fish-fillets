#include "subsystem/audio.h"

void AudioSource::mixin(float *output, std::size_t numSamples) {
    // TODO loop
    auto countRead = std::min(numSamples, m_samplesTotal - m_sampleIndex);
    for(auto i = 0u; i < countRead; i++)
        output[i] += m_data[m_sampleIndex++];
}

bool AudioSource::done() const {
    return m_sampleIndex == m_samplesTotal;
}

void AudioSource::rewind() {
    m_sampleIndex = 0;
}