#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

class AudioSource {
    class Impl {
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
        Impl() = delete;
        Impl(std::string filename, std::size_t num_samples, std::unique_ptr<float[]>&& data);

        Impl(Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
        Impl(Impl&&) = default;
        Impl& operator=(Impl&&) = default;
        ~Impl() { if(m_data) LOGD("freeing audio source: %s", m_name.c_str()); }

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

    std::shared_ptr<Impl> m_impl;

public:
    AudioSource() : m_impl() { }
    AudioSource(const std::string& filename, std::size_t num_samples, std::unique_ptr<float[]>&& data);

    void reset() { m_impl.reset(); }
    operator bool() const { return (bool)m_impl; }

    const std::string& name() const { return m_impl->name(); }
    float* data() const { return m_impl->data(); }
    bool isDialog() const { return m_impl->isDialog(); }

    void setLoop(std::size_t start = 0, std::size_t end = (std::size_t)(-1)) { m_impl->setLoop(start, end); }
    void setVolume(float volume) { m_impl->setVolume(volume); }
    void setDialog(bool isDialog) { m_impl->setDialog(isDialog); }

    bool done() const { return m_impl->done(); }
    void mixin(float output[], std::size_t numSamples) const { m_impl->mixin(output, numSamples); }
    void rewind() const { m_impl->rewind(); };
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H