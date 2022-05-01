#include "audio.h"
#include "files.h"
#include <numeric>

AudioStream::AudioStream(Instance* instance) {
    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(22050);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Medium);
    builder.setChannelCount(1);
    builder.setDataCallback(this);
    //builder.setErrorCallback(this);

    if(auto result = builder.openStream(&stream); result != oboe::Result::OK)
        ::error("Failed to open stream.",  "oboe: %s", convertToText(result));

    auto asset = instance->files->system("music/menu.ogg").asset();

    off64_t start, length;
    auto fd = AAsset_openFileDescriptor64(asset, &start, &length);
    extractor = AMediaExtractor_new();
    if(AMediaExtractor_setDataSourceFd(extractor, fd, start, length) != AMEDIA_OK)
        ::error("AMediaExtractor_setDataSourceFd failed");

    AMediaFormat *format = AMediaExtractor_getTrackFormat(extractor, 0);
    int32_t sampleRate;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate))
        ::error("AMediaFormat_getInt32 failed (sample rate)");
    LOGD("Source sample rate %d", sampleRate);
    assert(sampleRate == 22050);

    int32_t channelCount;
    if (!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount))
        ::error("AMediaFormat_getInt32 failed (channel count)");
    LOGV("Got channel count %d", channelCount);
    assert(channelCount == 1);

    const char *formatStr = AMediaFormat_toString(format);
    LOGV("Output format %s", formatStr);

    const char *mimeType;
    if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mimeType))
        ::error("AMediaFormat_getString failed (MIME type)");
    LOGV("Got mime type %s", mimeType);

    AMediaExtractor_selectTrack(extractor, 0);
    codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_configure(codec, format, nullptr, nullptr, 0);
    AMediaCodec_start(codec);

    bool extractorDone = false, codecDone = false;
    std::vector<std::uint8_t> output;

    do {
        auto inIndex = AMediaCodec_dequeueInputBuffer(codec, 2000);
        if(inIndex >= 0) {
            std::size_t inputSize;
            auto *inputBuffer = AMediaCodec_getInputBuffer(codec, inIndex, &inputSize);

            auto sampleSize = AMediaExtractor_readSampleData(extractor, inputBuffer, inputSize);
            auto presentationTimeUs = AMediaExtractor_getSampleTime(extractor);

            if(sampleSize > 0) {
                LOGV("extractor: sample size %ld", sampleSize);
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, sampleSize, presentationTimeUs, 0);
                AMediaExtractor_advance(extractor);
            } else {
                LOGV("Extraction finished");
                AMediaCodec_queueInputBuffer(codec, inIndex, 0, 0, presentationTimeUs, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                extractorDone = true;
            }
        } else if(inIndex == AMEDIACODEC_INFO_TRY_AGAIN_LATER)
            LOGV("in: try again later");
        else
            ::error("AMediaCodec_dequeueInputBuffer failed", "inIndex = %d", inIndex);

        AMediaCodecBufferInfo info;
        auto outIndex = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
        if(outIndex >= 0) {
            if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM){
                LOGV("Decoding finished");
                codecDone = true;
            }

            std::size_t outBufferSize;
            std::uint8_t *outBuffer = AMediaCodec_getOutputBuffer(codec, outIndex, &outBufferSize);
            LOGV("codec: buffer size %d", info.size);
            auto prevSize = output.size();
            output.resize(prevSize + info.size);
            std::memcpy(output.data() + prevSize, outBuffer, info.size);
            AMediaCodec_releaseOutputBuffer(codec, outIndex, false);
        } else if(outIndex == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
            LOGV("out: try again later");
        } else if(outIndex == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
            LOGV("out: buffer changed");
        } else if(outIndex == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            LOGV("out: format changed");
        } else
            ::error("AMediaCodec_dequeueOutputBuffer failed", "outIndex = %d", outIndex);
    } while(!(extractorDone && codecDone));
    LOGD("decoded %ld bytes", output.size());

    AMediaFormat_delete(format);
    AMediaCodec_delete(codec);
    AMediaExtractor_delete(extractor);

    auto numSamples = output.size() / 2;
    auto data16 = reinterpret_cast<std::int16_t*>(output.data());

    dataF = std::make_unique<float[]>(numSamples);
    oboe::convertPcm16ToFloat(data16, dataF.get(), (std::int32_t)numSamples);

    oboe::Result result = stream->requestStart();
    if (result != oboe::Result::OK)
        LOGE("Failed to start audio stream. Error: %s", convertToText(result));
}

AudioStream::~AudioStream() {
    stream->stop();
    stream->close();
}

oboe::DataCallbackResult
AudioStream::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
    //LOGD("audio ready for %d frames", numFrames);
    static std::size_t sample = 0;
    float* outData = reinterpret_cast<float*>(audioData);
    for(int i = 0; i < numFrames; i++) {
        outData[i] = dataF[sample];
        if(++sample >= 826780) {
            LOGD("Looping!");
            sample = 419772;
        }
    }
    return oboe::DataCallbackResult::Continue;
}
