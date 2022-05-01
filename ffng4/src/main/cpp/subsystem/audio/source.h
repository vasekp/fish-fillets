#ifndef FISH_FILLETS_AUDIO_SOURCE_H
#define FISH_FILLETS_AUDIO_SOURCE_H

class AudioSource {
    std::string _name;
    std::size_t samplesTotal;
    std::size_t sampleIndex;
    std::unique_ptr<float[]> data;

public:
    AudioSource(const std::string& filename, std::size_t num_samples, std::unique_ptr<float[]>&& data) :
            _name(filename),
            samplesTotal(num_samples),
            sampleIndex(0),
            data(std::move(data))
    { }

    AudioSource(AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    AudioSource(AudioSource&&) = default;
    AudioSource& operator=(AudioSource&&) = default;
    ~AudioSource() { if(data) LOGD("freeing audio source: %s", _name.c_str()); }

    const std::string& name() const { return _name; }

    bool done() const;
    void mixin(float output[], std::size_t numSamples);
};

#endif //FISH_FILLETS_AUDIO_SOURCE_H