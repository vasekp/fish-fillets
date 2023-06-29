#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/persist.h"

#include <sstream>

class AudioPreloader : public ScriptReferrer {
    Audio& m_audio;
    Script m_script;

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
};

Audio::Audio(Instance& instance) : m_instance(instance) {
    AudioPreloader(*this, instance).load();
    m_volumes[(int)AudioType::talk] = (float)instance.persist().get("volume_talk", 100) / 100.f;
    m_volumes[(int)AudioType::sound] = (float)instance.persist().get("volume_sound", 100) / 100.f;
    m_volumes[(int)AudioType::music] = (float)instance.persist().get("volume_music", 30) / 100.f;
}

void Audio::addSource(const AudioSourceBase::Ref& source) {
    Log::debug("adding audio source ", source.get(), " (", source->name(), ")");
    auto sources = m_sources.local();
    sources->push_back(source);
    sources.checkDialogs();
}

void Audio::removeSource(const AudioSourceBase::Ref& source) {
    auto sources = m_sources.local();
    auto it = std::find(sources->begin(), sources->end(), source);
    if(it == sources->end())
        Log::debug("removeSource: did not match");
    else {
        Log::debug("removing audio source ", source.get(), " (", source->name(), ")");
        sources->erase(it);
    }
    sources.checkDialogs();
}

void Audio::clear() {
    auto sources = m_sources.local();
    Log::debug("clear: ", sources->size(), " -> 0");
    sources->clear();
    sources.checkDialogs();
}

void Audio::clearExcept(const AudioSourceBase::Ref& source) {
    auto sources = m_sources.local();
    std::erase_if(sources.vector(), [&source](const auto& other) { return other != source; });
    sources.checkDialogs();
}

void Audio::preload(const std::string& filename) {
    m_sounds_preload.insert({filename, loadSound(filename)});
}

float Audio::getVolume(AudioType type) {
    return m_volumes[(int)type];
}

void Audio::setVolume(AudioType type, float volume) {
    m_volumes[(int)type] = volume;
}

bool Audio::isDialog() const {
    return m_sources.hasDialogs();
}

void Audio::mix(float* output, std::size_t numSamples) {
    if(numSamples == 0)
        return;
    std::array<float, 3> volumes;
    for(int i = 0; i < 3; i++)
        volumes[i] = m_volumes[i]; // cache atomics
    std::memset(output, 0, sizeof(float) * numSamples);
    auto& sources = m_sources.thread();
    unsigned dialogs = 0;
    for(const auto& source : sources) {
        if(source->done())
            continue;
        if(source->isDialog())
            dialogs++;
        source->mixin(output, numSamples, volumes[(int)source->type()]);
    }
    m_sources.setDialogsThread(dialogs > 0);
    if(auto guard = m_sources.threadGuard()) {
        auto newEnd = std::remove_if(sources.begin(), sources.end(),
                [](auto& source) { return source->done(); });
        if(newEnd != sources.end()) {
            Log::debug("AudioSink: removing ", std::distance(newEnd, sources.end()), " sources");
            sources.erase(newEnd, sources.end());
        }
    }
}

AudioData::Ref Audio::loadSound(const std::string& filename) const {
    if(m_sounds_preload.contains(filename))
        return m_sounds_preload.at(filename);
    else
        return decoders::ogg(m_instance, filename);
}

AudioSource::Ref Audio::loadMusic(const std::string& filename) const {
    auto data = loadSound(filename);
    auto source = AudioSource::create(data, AudioType::music);
    auto meta = m_instance.files().system(filename + ".meta");
    if(meta->exists()) {
        auto contents = meta->read();
        std::istringstream iss{contents};
        std::size_t start, end;
        iss >> start >> end;
        source->setLoop(start, end);
    } else
        source->setLoop();
    return source;
}
