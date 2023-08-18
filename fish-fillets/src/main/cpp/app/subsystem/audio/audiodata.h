#ifndef FISH_FILLETS_AUDIO_DATA_H
#define FISH_FILLETS_AUDIO_DATA_H

#include "common.h"

class AudioData {
    enum class Private { tag };

public:
    AudioData(std::string&& filename, std::size_t numSamples, Private) :
            m_filename(std::move(filename)),
            m_data(std::make_unique<float[]>(numSamples)),
            m_samples(numSamples),
            m_loopStart(0)
    { }

    AudioData(AudioData&) = delete;
    AudioData& operator=(const AudioData&) = delete;
    AudioData(AudioData&&) noexcept = default;
    AudioData& operator=(AudioData&&) noexcept = default;
    ~AudioData() { if(m_data) Log::verbose<Log::audio>("freeing audio data: ", m_filename); }

    using Ref = std::shared_ptr<AudioData>;
    static Ref create(std::string filename, std::size_t numSamples) {
        return std::make_shared<AudioData>(std::move(filename), numSamples, Private::tag);
    }
    void setLoopStart(std::size_t loopStart) { m_loopStart = loopStart; }

    const std::string& filename() const { return m_filename; }
    float* data() { return m_data.get(); }
    const float* data() const { return m_data.get(); }
    float operator[](std::size_t index) const { return m_data[index]; }
    std::size_t samples() const { return m_samples; }
    std::size_t loopStart() const { return m_loopStart; }

private:
    std::string m_filename;
    std::unique_ptr<float[]> m_data;
    std::size_t m_samples;
    std::size_t m_loopStart;
};

#endif //FISH_FILLETS_AUDIO_DATA_H
