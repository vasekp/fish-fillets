#ifndef FISH_FILLETS_AUDIO_DATA_H
#define FISH_FILLETS_AUDIO_DATA_H

class AudioData {
    std::string m_filename;
    std::size_t m_samples;
    std::unique_ptr<float[]> m_data;

    enum class Private { tag };

public:
    AudioData(std::string&& filename, std::size_t numSamples, Private) :
            m_filename(std::move(filename)), m_samples(numSamples), m_data(std::make_unique<float[]>(numSamples)) { }

    using Ref = std::shared_ptr<AudioData>;
    static Ref create(std::string filename, std::size_t numSamples) {
        return std::make_shared<AudioData>(std::move(filename), numSamples, Private::tag);
    }

    AudioData(AudioData&) = delete;
    AudioData& operator=(const AudioData&) = delete;
    AudioData(AudioData&&) = default;
    AudioData& operator=(AudioData&&) = default;
    ~AudioData() { if(m_data) LOGD("freeing audio data: %s", m_filename.c_str()); }

    const std::string& filename() const { return m_filename; }
    float* data() { return m_data.get(); }
    const float* data() const { return m_data.get(); }
    float operator[](std::size_t index) const { return m_data[index]; }
    std::size_t samples() const { return m_samples; }
};

#endif //FISH_FILLETS_AUDIO_DATA_H