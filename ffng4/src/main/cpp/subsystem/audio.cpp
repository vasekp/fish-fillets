#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"

//#include <media/NdkMediaExtractor.h> // XXX
#include <future>
#include <thread>
#include <sstream>
#include <cstring> // memset

class AudioPreloader : public ScriptReferrer {
    Instance& m_instance;
    Audio& m_audio;
    Script m_script;

public:
    explicit AudioPreloader(Instance& instance) : m_instance(instance), m_audio(instance.audio()), m_script(instance, *this) {
        m_script.registerFn("preload_sound", lua::wrap<&AudioPreloader::preload_sound>);
    }

    void load() {
        m_script.loadFile(m_instance.files().system("script/preload.lua"));
    }

    void preload_sound(const std::string& filename) {
        m_audio.preload(filename);
    }
};

void Audio::activate() {
    Log::debug("audio: activate");
    m_stream = std::make_unique<AudioSink>(*this);

    if(m_sounds_preload.empty())
        AudioPreloader(m_instance).load();
}

void Audio::shutdown() {
    Log::debug("audio: shutdown");
    m_stream.reset();
}

void Audio::pause() {
    m_stream->stop();
}

void Audio::resume() {
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
    m_sounds_preload.insert({filename, loadSound(filename, false)});
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

void loadSoundAsync(const std::string &filename, std::promise<AudioData::Ref>& promise, Instance& instance);

AudioData::Ref Audio::loadSound(const std::string& filename, bool async) {
    if(m_sounds_preload.contains(filename))
        return m_sounds_preload.at(filename);
    std::promise<AudioData::Ref> promise;
    auto future = promise.get_future();
    if(async) {
        std::thread([this, filename, &promise]() {
            return loadSoundAsync(filename, promise, m_instance);
        }).detach();
        future.wait();
    } else {
        loadSoundAsync(filename, promise, m_instance);
        assert(future.valid());
    }
    return future.get();
}

AudioSource::Ref Audio::loadMusic(const std::string& filename, bool async) {
    auto data = loadSound(filename, async);
    auto source = AudioSource::from(data);
    auto meta = m_instance.files().system(filename + ".meta");
    if(meta.exists()) {
        auto contents = meta.read();
        std::istringstream iss{contents};
        std::size_t start, end;
        iss >> start >> end;
        source->setLoop(start, end);
    } else
        source->setLoop();
    source->setVolume(0.3f);
    return source;
}

void loadSoundAsync(const std::string &filename, std::promise<AudioData::Ref>& promise, Instance& instance) {
    auto ret = AudioData::create(filename, 0);
    promise.set_value(ret);
}

/*void loadSoundAsync(const std::string &filename, std::promise<AudioData::Ref>& promise, Instance& instance) { // XXX
    auto asset = instance.files().system(filename).asset();

    off64_t start, length;
    bool doubleSample;
    auto fd = AAsset_openFileDescriptor64(asset, &start, &length);
    AMediaExtractor *extractor = AMediaExtractor_new();
    if (AMediaExtractor_setDataSourceFd(extractor, fd, start, length) != AMEDIA_OK)
        Log::fatal("AMediaExtractor_setDataSourceFd failed");

    AMediaFormat *format = AMediaExtractor_getTrackFormat(extractor, 0);
    int32_t sampleRate;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate))
        Log::fatal("AMediaFormat_getInt32 failed (sample rate)");
    switch(sampleRate) {
        case 22050:
            doubleSample = false;
            break;
        case 11025:
            doubleSample = true;
            break;
        default:
            Log::fatal("Unexpected audio sample rate (", filename, "): ", sampleRate);
    }

    int32_t channelCount;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount))
        Log::fatal("AMediaFormat_getInt32 failed (channel count)");
    if (channelCount != 1)
        Log::fatal("Unexpected channel count (", filename, "), expected mono: ", channelCount);

    const char *mimeType;
    if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mimeType))
        Log::fatal("AMediaFormat_getString failed (MIME type)");

    AMediaExtractor_selectTrack(extractor, 0);
    AMediaCodec *codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_configure(codec, format, nullptr, nullptr, 0);
    AMediaCodec_start(codec);
    Log::verbose("input format: ", AMediaFormat_toString(format));

    bool extractorDone = false, codecDone = false;
    std::int64_t duration;
    AMediaFormat_getInt64(format, AMEDIAFORMAT_KEY_DURATION, &duration);
    std::size_t numSamples = 1 + duration * 22050 / 1000000;
    assert(numSamples > 0);
    Log::verbose("numSamples: ", numSamples);
    std::int64_t curSample = 0;

    auto ret = AudioData::create(filename, numSamples);
    auto data = ret->data();
    promise.set_value(ret);

    do {
        auto inIndex = AMediaCodec_dequeueInputBuffer(codec, 0);
        if (inIndex >= 0) {
            std::size_t inputSize;
            auto *inputBuffer = AMediaCodec_getInputBuffer(codec, inIndex, &inputSize);

            auto sampleSize = AMediaExtractor_readSampleData(extractor, inputBuffer, inputSize);
            auto presentationTimeUs = AMediaExtractor_getSampleTime(extractor);

            if (sampleSize > 0) {
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, sampleSize, presentationTimeUs, 0);
                AMediaExtractor_advance(extractor);
            } else {
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, 0, presentationTimeUs, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                extractorDone = true;
            }
        } else if (inIndex != AMEDIACODEC_INFO_TRY_AGAIN_LATER)
            Log::fatal("AMediaCodec_dequeueInputBuffer failed: inIndex = ", inIndex);

        AMediaCodecBufferInfo info;
        auto outIndex = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
        if (outIndex >= 0) {
            if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM)
                codecDone = true;

            auto samplesRead = info.size / 2;
            std::size_t outBufferSize;
            std::uint8_t *outBuffer = AMediaCodec_getOutputBuffer(codec, outIndex, &outBufferSize);
            assert(curSample + samplesRead <= numSamples);
            oboe::convertPcm16ToFloat(reinterpret_cast<std::int16_t*>(outBuffer), &data[curSample], samplesRead);
            if(doubleSample) {
                for(auto i = samplesRead - 1; i >= 0; i--) {
                    float curr = data[curSample + i];
                    float prev = i > 0 ? data[curSample + i - 1] :
                                 curSample > 0 ? data[curSample - 2] : 0.f;
                    data[curSample + 2 * i] = curr;
                    data[curSample + 2 * i - 1] = (curr + prev) / 2.f;
                }
                curSample += 2 * samplesRead;
            } else
                curSample += samplesRead;
            AMediaCodec_releaseOutputBuffer(codec, outIndex, false);
        } else
            switch (outIndex) {
                case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED: {
                    auto ff = AMediaCodec_getOutputFormat(codec);
                    Log::verbose("output format: ", AMediaFormat_toString(ff));
                    AMediaFormat_delete(ff);
                }
                case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
                case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                    break;
                default:
                    Log::fatal("AMediaCodec_dequeueOutputBuffer failed: outIndex = ", outIndex);
            }
    } while (!(extractorDone && codecDone));
    numSamples = curSample;
    Log::debug("loadSound ", filename, ": decoded ", curSample, " frames");

    AMediaFormat_delete(format);
    AMediaCodec_delete(codec);
    AMediaExtractor_delete(extractor);
}*/
