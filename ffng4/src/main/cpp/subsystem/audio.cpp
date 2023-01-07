#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"

#include <sstream>
#include <cstring> // memset

class AudioPreloader : public ScriptReferrer {
    Audio& m_audio;
    Instance& m_instance;
    Script m_script;

public:
    explicit AudioPreloader(Audio& audio, Instance& instance) : m_audio(audio), m_instance(instance), m_script(instance, *this) {
        m_script.registerFn("preload_sound", lua::wrap<&AudioPreloader::preload_sound>);
    }

    void load() {
        m_script.loadFile(m_instance.files().system("script/preload.lua").get());
    }

    void preload_sound(const std::string& filename) {
        m_audio.preload(filename);
    }
};

Audio::Audio(Instance& instance) : m_instance(instance) {
    AudioPreloader(*this, instance).load();
}

void Audio::bindSink(AudioSink* sink) {
    Log::debug("audio: bind");
    m_stream = sink;
}

void Audio::unbindSink() {
    Log::debug("audio: unbind");
    m_stream = nullptr;
}

void Audio::pause() {
    if(m_stream)
        m_stream->stop();
}

void Audio::resume() {
    if(m_stream)
        m_stream->start();
}

void Audio::addSource(const AudioSource::Ref& source) {
    Log::debug("adding audio source ", source.get(), " (", source->filename(), ")");
    auto sources = m_sources.local();
    sources->push_back(source);
    sources.checkDialogs();
}

void Audio::removeSource(const AudioSource::Ref& source) {
    auto sources = m_sources.local();
    auto it = std::find(sources->begin(), sources->end(), source);
    if(it == sources->end())
        Log::debug("removeSource: did not match");
    else {
        Log::debug("removing audio source ", source.get(), " (", source->filename(), ")");
        sources->erase(it);
    }
    sources.checkDialogs();
}

void Audio::clear() {
    auto sources = m_sources.local();
    sources->clear();
    sources.checkDialogs();
}

void Audio::clearExcept(const AudioSource::Ref& source) {
    auto sources = m_sources.local();
    std::erase_if(sources.vector(), [&source](const auto& other) { return other != source; });
    sources.checkDialogs();
}

void Audio::preload(const std::string& filename) {
    m_sounds_preload.insert({filename, loadSound(filename)});
}

bool Audio::isDialog() const {
    return m_sources.hasDialogs();
}

void Audio::mix(float* output, std::size_t numSamples) {
    std::memset(output, 0, sizeof(float) * numSamples);
    auto& sources = m_sources.thread();
    unsigned dialogs = 0;
    for(const auto& source : sources) {
        if(source->done())
            continue;
        if(source->isDialog())
            dialogs++;
        source->mixin(output, numSamples);
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
        return loadOGG(filename);
}

AudioSource::Ref Audio::loadMusic(const std::string& filename) const {
    auto data = loadSound(filename);
    auto source = AudioSource::from(data);
    auto meta = m_instance.files().system(filename + ".meta");
    if(meta->exists()) {
        auto contents = meta->read();
        std::istringstream iss{contents};
        std::size_t start, end;
        iss >> start >> end;
        source->setLoop(start, end);
    } else
        source->setLoop();
    source->setVolume(0.3f);
    return source;
}
