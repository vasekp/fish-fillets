#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "sstream"

#include <media/NdkMediaExtractor.h>

void Audio::activate() {
    LOGD("audio: activate");
    m_stream = std::make_unique<AudioStream>(this);
}

void Audio::shutdown() {
    LOGD("audio: shutdown");
    m_stream.reset();
}

void Audio::addSource(std::shared_ptr<AudioSource>& source) {
    LOGD("adding audio source %s", source->name().c_str());
    m_sources.push_back(source);
}

void Audio::removeSource(const std::string &name) {
    auto newEnd = std::remove_if(m_sources.begin(), m_sources.end(), [&](const auto& source) { return source->name() == name; });
    LOGD("removeSource: name matched %ld sources", std::distance(newEnd, m_sources.end()));
    m_sources.erase(newEnd, m_sources.end());
}

void Audio::clear() {
    m_sources.clear();
}

AudioSource Audio::loadSound(const std::string& filename) {
    auto asset = m_instance->files->system(filename).asset();

    off64_t start, length;
    auto fd = AAsset_openFileDescriptor64(asset, &start, &length);
    AMediaExtractor* extractor = AMediaExtractor_new();
    if(AMediaExtractor_setDataSourceFd(extractor, fd, start, length) != AMEDIA_OK)
        ::error("AMediaExtractor_setDataSourceFd failed");

    AMediaFormat *format = AMediaExtractor_getTrackFormat(extractor, 0);
    int32_t sampleRate;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate))
        ::error("AMediaFormat_getInt32 failed (sample rate)");
    if(sampleRate != 22050)
        ::error("Bad sample rate.", "Unexpected audio sample rate (%s): %d", filename.c_str(), sampleRate);

    int32_t channelCount;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount))
        ::error("AMediaFormat_getInt32 failed (channel count)");
    if(channelCount != 1)
        ::error("Bad channel count.", "Unexpected channel count (%s), expected mono: %d", filename.c_str(), channelCount);

    const char *mimeType;
    if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mimeType))
        ::error("AMediaFormat_getString failed (MIME type)");

    AMediaExtractor_selectTrack(extractor, 0);
    AMediaCodec* codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_configure(codec, format, nullptr, nullptr, 0);
    AMediaCodec_start(codec);

    bool extractorDone = false, codecDone = false;
    std::vector<std::uint8_t> dataRaw;

    do {
        auto inIndex = AMediaCodec_dequeueInputBuffer(codec, 2000);
        if(inIndex >= 0) {
            std::size_t inputSize;
            auto *inputBuffer = AMediaCodec_getInputBuffer(codec, inIndex, &inputSize);

            auto sampleSize = AMediaExtractor_readSampleData(extractor, inputBuffer, inputSize);
            auto presentationTimeUs = AMediaExtractor_getSampleTime(extractor);

            if(sampleSize > 0) {
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, sampleSize, presentationTimeUs, 0);
                AMediaExtractor_advance(extractor);
            } else {
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, 0, presentationTimeUs, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                extractorDone = true;
            }
        } else if(inIndex != AMEDIACODEC_INFO_TRY_AGAIN_LATER)
            ::error("AMediaCodec_dequeueInputBuffer failed", "inIndex = %d", inIndex);

        AMediaCodecBufferInfo info;
        auto outIndex = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
        if(outIndex >= 0) {
            if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM)
                codecDone = true;

            std::size_t outBufferSize;
            std::uint8_t *outBuffer = AMediaCodec_getOutputBuffer(codec, outIndex, &outBufferSize);
            auto prevSize = dataRaw.size();
            dataRaw.resize(prevSize + info.size);
            std::memcpy(dataRaw.data() + prevSize, outBuffer, info.size);
            AMediaCodec_releaseOutputBuffer(codec, outIndex, false);
        } else switch(outIndex) {
                case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
                case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
                    break;
                default:
                    ::error("AMediaCodec_dequeueOutputBuffer failed", "outIndex = %d", outIndex);
            }
    } while(!(extractorDone && codecDone));
    LOGD("loadSound %s: decoded %ld bytes", filename.c_str(), dataRaw.size());

    AMediaFormat_delete(format);
    AMediaCodec_delete(codec);
    AMediaExtractor_delete(extractor);

    std::size_t numSamples = dataRaw.size() / 2;
    auto data = std::make_unique<float[]>(numSamples);
    oboe::convertPcm16ToFloat(reinterpret_cast<std::int16_t*>(dataRaw.data()), data.get(), (std::int32_t)numSamples);

    return {filename, numSamples, std::move(data)};
}

AudioSource Audio::loadMusic(const std::string& filename) {
    auto ret = loadSound(filename);
    auto meta = m_instance->files->system(filename + ".meta");
    if(meta.exists()) {
        auto contents = meta.read();
        std::istringstream iss{contents};
        std::size_t start, end;
        iss >> start >> end;
        ret.setLoop(start, end);
    } else
        ret.setLoop();
    return ret;
}