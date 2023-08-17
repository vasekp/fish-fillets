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

void DataAudioSource::mixin(float* output, std::size_t numSamples, float refVolume) {
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
    m_head(std::make_unique<Node>()), m_tail(m_head.get()),
    m_curNode(m_head.get()), m_curIndex(0),
    m_total(0),
    m_name(std::move(name))
{ }

void AudioSourceQueue::enqueue(std::vector<float>&& data) {
    m_total += data.size();
    auto* tail = m_tail.load(std::memory_order::acquire);
    tail->next = std::make_unique<Node>();
    tail->next->data = std::move(data);
    m_tail.store(tail->next.get(), std::memory_order::release);

    // Good opportunity to clean up.
    auto* curNode = m_curNode.load(std::memory_order::acquire);
    if(!curNode) {
        Log::error("AudioSourceQueue: lost audio sync");
        return;
    }
    while(m_head.get() != curNode)
        m_head = std::move(m_head->next);
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
        if(current == m_tail.load(std::memory_order::acquire)) {
            Log::debug<Log::audio>("Audio data ended.");
            m_curNode.store(nullptr, std::memory_order::release);
        } else {
            m_curNode.store(current->next.get(), std::memory_order::release);
            m_curIndex = 0;
            mixin(output + countRead, numSamples - countRead, refVolume);
        }
    }
}

bool AudioSourceQueue::done() const {
    return !m_curNode.load(std::memory_order::acquire);
}
