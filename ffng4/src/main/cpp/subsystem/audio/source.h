#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

class AudioSource {
    std::string name;
    std::size_t samplesTotal;
    std::size_t sampleIndex;
    std::unique_ptr<float[]> data;

public:
    AudioSource(const std::string& filename, std::size_t num_samples, std::unique_ptr<float[]>&& data) :
        name(filename),
        samplesTotal(num_samples),
        sampleIndex(0),
        data(std::move(data))
    { }

    AudioSource(AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    AudioSource(AudioSource&&) = default;
    AudioSource& operator=(AudioSource&&) = default;
    ~AudioSource() { LOGD("freeing audio source: %s", name.c_str()); }

    bool done() const;
    void mixin(float output[], std::size_t numSamples);
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H