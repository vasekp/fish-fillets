#include "subsystem/audio.h"

void AudioSource::mixin(float *output, std::size_t numSamples) {
    // TODO loop
    auto countRead = std::min(numSamples, samplesTotal - sampleIndex);
    for(auto i = 0u; i < countRead; i++)
        output[i] += data[sampleIndex++];
}

bool AudioSource::done() const {
    return sampleIndex == samplesTotal;
}