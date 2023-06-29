#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

enum class AudioType {
    sound,
    music,
    talk
};

class AudioSourceBase { // TODO rename
protected:
    AudioType m_type;
    float m_volume = 1.0f;
    float m_dialog = false;

public:
    AudioSourceBase(AudioType type) : m_type(type) { }
    virtual ~AudioSourceBase() { }

    void setVolume(float volume) { m_volume = volume; }
    void setDialog(bool isDialog) { m_dialog = isDialog; }
    bool isDialog() const { return m_dialog; }
    AudioType type() const { return m_type; }
    virtual std::string_view name() const = 0;
    virtual void mixin(float output[], std::size_t numSamples, float refVolume) = 0;
    virtual bool done() const = 0;
    using Ref = std::shared_ptr<AudioSourceBase>;
};

class AudioSource : public AudioSourceBase {
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

    enum class Private { tag };

public:
    AudioSource(AudioData::Ref data, AudioType type, Private);

    using Ref = std::shared_ptr<AudioSource>;
    static Ref create(const AudioData::Ref&, AudioType type);

    const auto& filename() const { return m_data->filename(); }
    std::string_view name() const override { return filename(); }
    void mixin(float output[], std::size_t numSamples, float refVolume) override;
    bool done() const override;

    void setLoop(std::size_t start = 0, std::size_t end = (std::size_t)(-1));
};

class AudioSourceQueue : public AudioSourceBase {
    struct Node {
        std::vector<float> data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> m_start;
    std::atomic<Node*> m_last;
    std::size_t m_curIndex;
    std::size_t m_total;
    std::string m_name;

public:
    AudioSourceQueue(std::string name, AudioType type);

    using Ref = std::shared_ptr<AudioSourceQueue>;

    void enqueue(std::vector<float>&& data);
    std::string_view name() const override { return m_name; }
    void mixin(float output[], std::size_t numSamples, float refVolume) override;
    bool done() const override;

    std::size_t total() const { return m_total; }
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H