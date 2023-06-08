#include <utility>

#include "subsystem/audio.h"
#include "source.h"

AudioSource::AudioSource(AudioData::Ref data, Private) :
    m_data(std::move(data)),
    m_samplesTotal(m_data->samples()),
    m_sampleIndex(),
    m_loop(false),
    m_loopStart(0),
    m_loopEnd(0)
{ }

AudioSource::Ref AudioSource::from(const AudioData::Ref& data) {
    return std::make_shared<AudioSource>(data, Private::tag);
}

void AudioSource::mixin(float *output, std::size_t numSamples) {
    auto countRead = std::min(numSamples, m_samplesTotal - m_sampleIndex);
    for (auto i = 0u; i < countRead; i++)
        output[i] += m_volume * (*m_data)[m_sampleIndex++]; // TODO m_volume → volume()
    if(m_loop && m_sampleIndex >= m_loopEnd) {
        Log::debug("music loop");
        m_sampleIndex = m_loopStart;
        mixin(output + countRead, numSamples - countRead);
    }
}

void AudioSource::setLoop(std::size_t start, std::size_t end) {
    m_loop = (end != start);
    m_loopStart = start;
    m_loopEnd = std::min(end, m_samplesTotal);
    Log::verbose("setLoop: ", (int)m_loopStart, " -- ", (int)m_loopEnd);
}

bool AudioSource::done() const {
    return m_sampleIndex == m_samplesTotal;
}

AudioSourceQueue::AudioSourceQueue(std::string name) :
    m_start(), m_last(),
    m_curIndex(0), m_total(0),
    m_name(std::move(name))
{ }

void AudioSourceQueue::enqueue(std::vector<float>&& data) {
    m_total += data.size();
    if(Node* last = m_last.load(); !last) {
        m_start = std::make_unique<Node>();
        m_start->data = std::move(data);
        m_last.store(m_start.get());
    } else {
        last->next = std::make_unique<Node>();
        last->next->data = std::move(data);
        m_last.store(last->next.get());
    }
}

void AudioSourceQueue::mixin(float *output, std::size_t numSamples) {
    // assert m_start
    const auto& data = m_start->data;
    if(m_curIndex + numSamples < data.size()) {
        for(auto i = 0u; i < numSamples; i++)
            output[i] += m_volume * data[m_curIndex++];
    } else {
        auto countRead = data.size() - m_curIndex;
        for(auto i = 0u; i < countRead; i++)
            output[i] += m_volume * data[m_curIndex++];
        m_start = std::move(m_start->next);
        if(m_start) {
            m_curIndex = 0;
            mixin(output + countRead, numSamples - countRead);
        } else
            Log::info("Audio data ended.");
    }
}

bool AudioSourceQueue::done() const {
    return !m_start;
}
