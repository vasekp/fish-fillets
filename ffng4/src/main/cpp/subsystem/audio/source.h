#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

class AudioSource {
    std::string m_name;
    std::size_t m_samplesTotal;
    std::size_t m_sampleIndex;
    std::unique_ptr<float[]> m_data;

public:
    AudioSource(const std::string& filename, std::size_t num_samples, std::unique_ptr<float[]>&& data) :
            m_name(filename),
            m_samplesTotal(num_samples),
            m_sampleIndex(0),
            m_data(std::move(data))
    { }

    AudioSource(AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    AudioSource(AudioSource&&) = default;
    AudioSource& operator=(AudioSource&&) = default;
    ~AudioSource() { if(m_data) LOGD("freeing audio source: %s", m_name.c_str()); }

    const std::string& name() const { return m_name; }

    bool done() const;
    void mixin(float output[], std::size_t numSamples);
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H