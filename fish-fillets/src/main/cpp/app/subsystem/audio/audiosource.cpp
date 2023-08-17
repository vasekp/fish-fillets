#include "subsystem/audio.h"

AudioSource::Ref AudioSource::create(const AudioData::Ref& data, AudioType type) {
    return std::make_shared<DataAudioSource>(data, type);
}

DataAudioSource::DataAudioSource(AudioData::Ref data, AudioType type) :
    AudioSource(type),
    m_data(std::move(data)),
    m_samplesTotal(m_data->samples()),
    m_sampleIndex(),
    m_repeat(false)
{ }

void DataAudioSource::mixin(float *output, std::size_t numSamples, float refVolume) {
    auto countRead = std::min(numSamples, m_samplesTotal - m_sampleIndex);
    float volume = refVolume * m_volume;
    for (auto i = 0u; i < countRead; i++)
        output[i] += volume * (*m_data)[m_sampleIndex++];
    if(m_sampleIndex == m_samplesTotal && m_repeat) {
        Log::debug<Log::audio>("audio loop");
        m_sampleIndex = m_data->loopStart();
        mixin(output + countRead, numSamples - countRead, refVolume);
    }
}

bool DataAudioSource::done() const {
    return m_sampleIndex == m_samplesTotal;
}

AudioSourceQueue::AudioSourceQueue(std::string name, AudioType type) :
    AudioSource(type),
    m_head(), m_tail(),
    m_curNode(), m_curIndex(0),
    m_total(0),
    m_name(std::move(name))
{ }

void AudioSourceQueue::enqueue(std::vector<float>&& data) {
    m_total += data.size();
    if(!m_head) {
        m_head = std::make_unique<Node>();
        m_head->data = std::move(data);
        m_curNode = m_tail = m_head.get();
    } else {
        m_tail->next = std::make_unique<Node>();
        m_tail->next->data = std::move(data);
        m_tail = m_tail->next.get();

        // Good opportunity to clean up.
        auto* curNode = m_curNode.load(std::memory_order::acquire);
        while(m_head.get() != curNode)
            m_head = std::move(m_head->next);
    }
}

void AudioSourceQueue::mixin(float* output, std::size_t numSamples, float refVolume) {
    auto* current = m_curNode.load(std::memory_order::acquire);
    if(!current) {
        Log::error("AudioSourceQueue: data not ready");
        return;
    }
    const auto& data = current->data;
    float volume = refVolume * m_volume;
    if(m_curIndex + numSamples < data.size()) {
        for(auto i = 0u; i < numSamples; i++)
            output[i] += volume * data[m_curIndex++];
    } else {
        auto countRead = data.size() - m_curIndex;
        for(auto i = 0u; i < countRead; i++)
            output[i] += volume * data[m_curIndex++];
        current = current->next.get();
        m_curNode.store(current, std::memory_order::release);
        if(current) {
            m_curIndex = 0;
            mixin(output + countRead, numSamples - countRead, refVolume);
        } else
            Log::debug<Log::audio>("Audio data ended.");
    }
}

bool AudioSourceQueue::done() const {
    return !m_curNode.load(std::memory_order::acquire);
}
