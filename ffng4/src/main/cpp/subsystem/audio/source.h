#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

class AudioSource {
    std::string m_name;
    std::size_t m_samplesTotal;
    mutable std::size_t m_sampleIndex;
    std::unique_ptr<float[]> m_data;
    bool m_loop;
    std::size_t m_loopStart;
    std::size_t m_loopEnd;
    float m_volume;
    bool m_dialog;

public:
    AudioSource() = delete;
    AudioSource(std::string filename, std::size_t num_samples, std::unique_ptr<float[]>&& data);

    AudioSource(AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    AudioSource(AudioSource&&) = default;
    AudioSource& operator=(AudioSource&&) = default;
    ~AudioSource() { if(m_data) LOGD("freeing audio source: %s", m_name.c_str()); }

    const std::string& name() const { return m_name; }
    float* data() const { return m_data.get(); }
    bool isDialog() const { return m_dialog; }

    void setLoop(std::size_t start = 0, std::size_t end = (std::size_t)(-1));
    void setVolume(float volume) { m_volume = volume; }
    void setDialog(bool isDialog) { m_dialog = isDialog; }

    bool done() const;
    void mixin(float output[], std::size_t numSamples) const;
    void rewind() const;
};

using AudioSourceRef = std::shared_ptr<AudioSource>;

#endif //FISH_FILLETS_AUDIO_SOURCE_H