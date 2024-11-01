#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

#include "common.h"
#include "audiotype.h"
#include "audiodata.h"

class AudioSource {
public:
    AudioSource(AudioType type) : m_type(type) { }
    virtual ~AudioSource() { }

    using Ref = std::shared_ptr<AudioSource>;
    static Ref create(const AudioData::Ref&, AudioType type);

    void setVolume(float volume) { m_volume = volume; }
    void setDialog(bool isDialog) { m_dialog = isDialog; }
    virtual void setRepeat(bool repeat) = 0;
    bool isDialog() const { return m_dialog; }
    AudioType type() const { return m_type; }
    virtual std::string_view name() const = 0;
    virtual void mixin(float output[], std::size_t numSamples, float refVolume) = 0;
    virtual bool done() const = 0;

protected:
    AudioType m_type;
    float m_volume = 1.0f;
    float m_dialog = false;
};

class DataAudioSource : public AudioSource {
public:
    DataAudioSource(AudioData::Ref data, AudioType type);
    DataAudioSource(const DataAudioSource&) = delete;
    DataAudioSource& operator=(const DataAudioSource&) = delete;

    std::string_view name() const override { return m_data->filename(); }
    void mixin(float output[], std::size_t numSamples, float refVolume) override;
    bool done() const override;

    void setRepeat(bool repeat) override { m_repeat = repeat; }

private:
    AudioData::Ref m_data;
    std::size_t m_samplesTotal;
    std::atomic<std::size_t> m_sampleIndex;
    bool m_repeat;
};

class AudioSourceQueue : public AudioSource {
public:
    AudioSourceQueue(std::string name, AudioType type);

    using Ref = std::shared_ptr<AudioSourceQueue>;

    void enqueue(std::vector<float>&& data);
    std::string_view name() const override { return m_name; }
    void mixin(float output[], std::size_t numSamples, float refVolume) override;
    bool done() const override;
    void setRepeat(bool repeat) override { std::unreachable(); }

    std::size_t total() const { return m_total; }

private:
    struct Node {
        std::vector<float> data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> m_head;
    std::atomic<Node*> m_tail;
    std::atomic<Node*> m_curNode;
    std::size_t m_curIndex;
    std::size_t m_total;
    std::string m_name;
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H
