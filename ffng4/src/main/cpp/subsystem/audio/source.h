#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

class AudioSource {
    AudioData::Ref m_data;
    std::size_t m_samplesTotal;
    class Index {
        std::size_t i;
    public:
        Index() : i(0) { }
        Index(const Index&) : i(0) { }
        Index& operator=(const Index&) { i = 0; return *this; }
        Index& operator=(std::size_t j) { i = j; return *this; }
        Index(Index&& other) : i(other.i) { }
        Index& operator=(Index&& other) { i = other.i; return *this; }
        operator std::size_t&() { return i; }
        operator std::size_t() const { return i; }
    } m_sampleIndex;

    bool m_loop;
    std::size_t m_loopStart;
    std::size_t m_loopEnd;
    float m_volume;
    bool m_dialog;

    enum class Private { tag };

public:
    AudioSource(AudioData::Ref data, Private);

    using Ref = std::shared_ptr<AudioSource>;
    static Ref from(const AudioData::Ref&);

    void setLoop(std::size_t start = 0, std::size_t end = (std::size_t)(-1));
    void setVolume(float volume) { m_volume = volume; }
    void setDialog(bool isDialog) { m_dialog = isDialog; }

    const auto& filename() const { return m_data->filename(); }
    bool isDialog() const { return m_dialog; }
    void mixin(float output[], std::size_t numSamples);
    bool done() const;
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H