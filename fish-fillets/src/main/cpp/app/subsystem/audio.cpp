#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/persist.h"

#include <sstream>

class AudioPreloader : public ScriptReferrer {
public:
    explicit AudioPreloader(Audio& audio, Instance& instance) : m_audio(audio), m_script(instance, *this) {
        m_script.registerFn("preload_sound", lua::wrap<&AudioPreloader::preload_sound>);
    }

    void load() {
        m_script.loadFile("script/preload.lua");
    }

    void preload_sound(const std::string& filename) {
        m_audio.preload(filename);
    }

private:
    Audio& m_audio;
    Script m_script;
};

Audio::Audio(Instance& instance) : m_instance(instance) {
    AudioPreloader(*this, instance).load();
    m_volumes[(int)AudioType::talk] = (float)instance.persist().get("volume_talk", 100) / 100.f;
    m_volumes[(int)AudioType::sound] = (float)instance.persist().get("volume_sound", 100) / 100.f;
    m_volumes[(int)AudioType::music] = (float)instance.persist().get("volume_music", 30) / 100.f;
}

void Audio::addSource(const AudioSource::Ref& source) {
    Log::debug<Log::audio>("adding audio source ", source.get(), " (", source->name(), ")");
    auto sources = m_sources.local();
    sources->push_back(source);
}

void Audio::removeSource(const AudioSource::Ref& source) {
    auto sources = m_sources.local();
    auto it = std::find(sources->begin(), sources->end(), source);
    if(it == sources->end())
        Log::debug<Log::audio>("removeSource: did not match");
    else {
        Log::debug<Log::audio>("removing audio source ", source.get(), " (", source->name(), ")");
        sources->erase(it);
    }
}

void Audio::ping() {
    auto sources = m_sources.local();
    auto newEnd = std::remove_if(sources->begin(), sources->end(),
            [](auto& source) { return source->done(); });
    if(newEnd != sources->end()) {
        Log::debug<Log::audio>("audio: removing ", std::distance(newEnd, sources->end()), " sources");
        sources->erase(newEnd, sources->end());
    }
}

void Audio::clear() {
    auto sources = m_sources.local();
    if(sources->size() > 0)
        Log::debug<Log::audio>("audio clear: ", sources->size(), " -> 0");
    sources->clear();
}

void Audio::clearExcept(const AudioSource::Ref& source) {
    auto sources = m_sources.local();
    std::erase_if(sources.vector(), [&source](const auto& other) { return other != source; });
}

void Audio::preload(const std::string& filename) {
    m_sounds_preload.insert({filename, loadSound(filename)});
}

float Audio::getVolume(AudioType type) {
    return m_volumes[(int)type].load(std::memory_order::relaxed);
}

void Audio::setVolume(AudioType type, float volume) {
    m_volumes[(int)type].store(volume, std::memory_order::relaxed);
}

bool Audio::isDialog() const {
    return m_sources.hasDialogs();
}

void Audio::mix(float* output, std::size_t numSamples) {
    if(numSamples == 0)
        return;
    std::array<float, 3> volumes;
    for(int i = 0; i < 3; i++)
        volumes[i] = m_volumes[i].load(std::memory_order::relaxed); // cache atomics
    std::memset(output, 0, sizeof(float) * numSamples);
    auto& sources = m_sources.thread();
    for(const auto& source : sources)
        if(!source->done())
            source->mixin(output, numSamples, volumes[(int)source->type()]);
}

AudioData::Ref Audio::loadSound(const std::string& filename) const {
    if(m_sounds_preload.contains(filename))
        return m_sounds_preload.at(filename);
    auto ref = decoders::ogg(m_instance, filename);
    auto meta = m_instance.files().system(filename + ".meta");
    if(meta->exists()) {
        auto contents = meta->read();
        std::istringstream iss{contents};
        std::size_t start;
        iss >> start;
        ref->setLoopStart(start);
    }
    return ref;
}

AudioSource::Ref Audio::loadMusic(const std::string& filename, bool repeat) const {
    auto data = loadSound(filename);
    auto source = AudioSource::create(data, AudioType::music);
    source->setRepeat(repeat);
    return source;
}
