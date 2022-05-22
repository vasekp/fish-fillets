#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "sstream"

#include <media/NdkMediaExtractor.h>
#include <future>
#include <thread>

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
        m_audio.m_sounds_preload.insert({filename, m_audio.loadSound(filename, false)});
    }
};

void Audio::activate() {
    LOGD("audio: activate");
    m_stream = std::make_unique<AudioStream>(*this);

    if(m_sounds_preload.empty())
        AudioPreloader(m_instance).load();
}

void Audio::shutdown() {
    LOGD("audio: shutdown");
    m_stream.reset();
}

void Audio::pause() {
    m_stream->stop();
}

void Audio::resume() {
    m_stream->start();
}

void Audio::addSource(AudioSourceRef source) {
    LOGD("adding audio source %s", source->name().c_str());
    source->rewind();
    auto sources = m_sources.local();
    sources->push_back(source);
    if(source->isDialog())
        sources.setDialogsLocal(true);
}

void Audio::removeSource(AudioSourceRef source) {
    auto sources = m_sources.local();
    auto it = std::find(sources->begin(), sources->end(), source);
    if(it == sources->end())
        LOGD("removeSource: did not match");
    else {
        LOGD("removing audio source %s", source->name().c_str());
        sources->erase(it);
    }
}

void Audio::clear() {
    m_sources.local()->clear();
}

bool Audio::isDialog() const {
    return m_sources.hasDialogs();
}

oboe::DataCallbackResult
Audio::onAudioReady(oboe::AudioStream*, void *audioData, int32_t numFrames) {
    auto outData = reinterpret_cast<float*>(audioData);
    std::memset(audioData, 0, sizeof(float) * numFrames);
    auto& sources = m_sources.thread();
    unsigned dialogs = 0;
    for(const auto& source : sources) {
        if(source->done())
            continue;
        if(source->isDialog())
            dialogs++;
        source->mixin(outData, numFrames);
    }
    m_sources.setDialogsThread(dialogs > 0);
    auto newEnd = std::remove_if(sources.begin(), sources.end(),
                                 [](auto& source) { return source->done(); });
    if(newEnd != sources.end()) {
        LOGD("AudioStream: removing %lu sources", std::distance(newEnd, sources.end()));
        sources.erase(newEnd, sources.end());
    }
    return oboe::DataCallbackResult::Continue;
}

void loadSoundAsync(const std::string &filename, std::promise<AudioSourceRef>& promise, Instance& instance);

AudioSourceRef Audio::loadSound(const std::string& filename, bool async) {
    if(m_sounds_preload.contains(filename))
        return m_sounds_preload.at(filename);
    std::promise<AudioSourceRef> promise;
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

AudioSourceRef Audio::loadMusic(const std::string& filename, bool async) {
    auto source = loadSound(filename, async);
    auto meta = m_instance.files().system(filename + ".meta");
    if(meta.exists()) {
        auto contents = meta.read();
        std::istringstream iss{contents};
        std::size_t start, end;
        iss >> start >> end;
        source->setLoop(start, end);
    } else
        source->setLoop();
    source->setVolume(0.3);
    return source;
}

void loadSoundAsync(const std::string &filename, std::promise<AudioSourceRef>& promise, Instance& instance) {
    auto asset = instance.files().system(filename).asset();

    off64_t start, length;
    auto fd = AAsset_openFileDescriptor64(asset, &start, &length);
    AMediaExtractor *extractor = AMediaExtractor_new();
    if (AMediaExtractor_setDataSourceFd(extractor, fd, start, length) != AMEDIA_OK)
        ::error("AMediaExtractor_setDataSourceFd failed");

    AMediaFormat *format = AMediaExtractor_getTrackFormat(extractor, 0);
    int32_t sampleRate;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate))
        ::error("AMediaFormat_getInt32 failed (sample rate)");
    /*if (sampleRate != 22050) // TODO
        ::error("Bad sample rate.", "Unexpected audio sample rate (%s): %d", filename.c_str(),
                sampleRate);*/

    int32_t channelCount;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount))
        ::error("AMediaFormat_getInt32 failed (channel count)");
    if (channelCount != 1)
        ::error("Bad channel count.", "Unexpected channel count (%s), expected mono: %d",
                filename.c_str(), channelCount);

    const char *mimeType;
    if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mimeType))
        ::error("AMediaFormat_getString failed (MIME type)");

    AMediaExtractor_selectTrack(extractor, 0);
    AMediaCodec *codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_configure(codec, format, nullptr, nullptr, 0);
    AMediaCodec_start(codec);
    LOGV("input format: %s", AMediaFormat_toString(format));

    bool extractorDone = false, codecDone = false;
    std::int64_t duration;
    AMediaFormat_getInt64(format, AMEDIAFORMAT_KEY_DURATION, &duration);
    std::size_t numSamples = 1 + duration * 22050 / 1000000;
    assert(numSamples > 0);
    LOGV("numSamples %ld", numSamples);
    std::int64_t curSample = 0;

    auto ret = std::make_shared<AudioSource>(filename, numSamples, std::make_unique<float[]>(numSamples));
    auto data = ret->data();
    promise.set_value(ret);
    return;

    do {
        auto inIndex = AMediaCodec_dequeueInputBuffer(codec, 2000);
        if (inIndex >= 0) {
            std::size_t inputSize;
            auto *inputBuffer = AMediaCodec_getInputBuffer(codec, inIndex, &inputSize);

            auto sampleSize = AMediaExtractor_readSampleData(extractor, inputBuffer, inputSize);
            auto presentationTimeUs = AMediaExtractor_getSampleTime(extractor);

            if (sampleSize > 0) {
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, sampleSize, presentationTimeUs,
                                             0);
                AMediaExtractor_advance(extractor);
            } else {
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, 0, presentationTimeUs,
                                             AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                extractorDone = true;
            }
        } else if (inIndex != AMEDIACODEC_INFO_TRY_AGAIN_LATER)
            ::error("AMediaCodec_dequeueInputBuffer failed", "inIndex = %d", inIndex);

        AMediaCodecBufferInfo info;
        auto outIndex = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
        if (outIndex >= 0) {
            if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM)
                codecDone = true;

            auto samplesRead = info.size / 2;
            std::size_t outBufferSize;
            std::uint8_t *outBuffer = AMediaCodec_getOutputBuffer(codec, outIndex,
                                                                  &outBufferSize);
            assert(curSample + samplesRead <= numSamples);
            oboe::convertPcm16ToFloat(reinterpret_cast<std::int16_t *>(outBuffer),
                                      &data[curSample], samplesRead);
            curSample += samplesRead;
            //std::memcpy(dataRaw.data() + prevSize, outBuffer, info.size);
            AMediaCodec_releaseOutputBuffer(codec, outIndex, false);
        } else
            switch (outIndex) {
                case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED: {
                    auto ff = AMediaCodec_getOutputFormat(codec);
                    LOGV("output format: %s", AMediaFormat_toString(ff));
                    AMediaFormat_delete(ff);
                }
                case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
                case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                    break;
                default:
                    ::error("AMediaCodec_dequeueOutputBuffer failed", "outIndex = %d",
                            outIndex);
            }
    } while (!(extractorDone && codecDone));
    LOGD("loadSound %s: decoded %ld frames", filename.c_str(), curSample);

    AMediaFormat_delete(format);
    AMediaCodec_delete(codec);
    AMediaExtractor_delete(extractor);
}