#include "subsystem/graphics.h"
#include "subsystem/audio/audiodata.h"
#include "files.h"
#include "ainstance.h"
#include <thread>

#include <android/bitmap.h>
#include <media/NdkMediaExtractor.h>

static AudioData::Ref loadSoundAsync(Instance& instance, const std::string& filename);

namespace decoders {
    ImageData png(Instance& instance, const std::string& filename0) {
        auto filename = dynamic_cast<SystemFile&>(*instance.files().system(filename0)).path();
        Log::debug<Log::graphics>("Load PNG ", filename);
        auto& jni = dynamic_cast<AndroidInstance&>(instance).jni;
        jstring jPath = jni->NewStringUTF(filename.c_str());
        jobject jBitmap = jni->CallObjectMethod(jni.object(), jni.getMethod("loadBitmap"), jPath);
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(jni, jBitmap, &info);
        unsigned width = info.width;
        unsigned height = info.height;
        unsigned stride = info.stride;
        void* pixels;
        AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
        if(!jBitmap)
            Log::fatal("bitmap data null (", filename, ")");
        if(stride != 4 * width)
            Log::error("PNG: system-provided stride ", stride, " ≠ 4*width ", 4 * width);
        auto dataSize = width * height * 4;
        auto data = std::make_unique<std::uint8_t[]>(dataSize);
        std::memcpy(data.get(), pixels, dataSize);
        AndroidBitmap_unlockPixels(jni, jBitmap);
        jni->DeleteLocalRef(jPath);
        jni->DeleteLocalRef(jBitmap);
        return {{width, height}, std::move(data)};
    }

    AudioData::Ref ogg(Instance& instance, const std::string& filename) {
        if(auto ret = loadSoundAsync(instance, filename))
            return ret;
        else {
            Log::warn("Replacing ", filename, " with silence");
            return AudioData::create(filename, 1000);
        }
    }
}

static AudioData::Ref loadSoundAsync(Instance& instance, const std::string& filename) {
    auto file = instance.files().system(filename);
    auto asset = dynamic_cast<SystemFile&>(*file).asset();

    off64_t start, length;
    bool doubleSample;
    auto fd = AAsset_openFileDescriptor64(asset, &start, &length);
    AMediaExtractor* extractor = AMediaExtractor_new();
    if(AMediaExtractor_setDataSourceFd(extractor, fd, start, length) != AMEDIA_OK) {
        Log::error("AMediaExtractor_setDataSourceFd failed");
        return {};
    }

    AMediaFormat* format = AMediaExtractor_getTrackFormat(extractor, 0);
    int32_t sampleRate;
    if(!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate)) {
        Log::error("AMediaFormat_getInt32 failed (sample rate)");
        return {};
    }
    switch(sampleRate) {
        case 22050:
            doubleSample = false;
            break;
        case 11025:
            doubleSample = true;
            break;
        default:
            Log::error("Unexpected audio sample rate (", filename, "): ", sampleRate);
            return {};
    }

    int32_t channelCount;
    if(!AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount)) {
        Log::error("AMediaFormat_getInt32 failed (channel count)");
        return {};
    }
    if(channelCount != 1) {
        Log::error("Unexpected channel count (", filename, "), expected mono: ", channelCount);
        return {};
    }

    const char* mimeType;
    if(!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mimeType)) {
        Log::error("AMediaFormat_getString failed (MIME type)");
        return {};
    }

    AMediaExtractor_selectTrack(extractor, 0);
    AMediaCodec* codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_configure(codec, format, nullptr, nullptr, 0);
    AMediaCodec_start(codec);
    Log::verbose<Log::audio>("input format: ", AMediaFormat_toString(format));

    bool extractorDone = false, codecDone = false;
    std::int64_t duration;
    AMediaFormat_getInt64(format, AMEDIAFORMAT_KEY_DURATION, &duration);
    std::size_t numSamples = 1 + duration * 22050 / 1000000;
    assert(numSamples > 0);
    Log::verbose<Log::audio>("numSamples: ", numSamples);
    std::int64_t curSample = 0;

    auto ret = AudioData::create(filename, numSamples);
    std::thread([=]() mutable {
        auto data = ret->data();
        do {
            auto inIndex = AMediaCodec_dequeueInputBuffer(codec, 0);
            if(inIndex >= 0) {
                std::size_t inputSize;
                auto* inputBuffer = AMediaCodec_getInputBuffer(codec, inIndex, &inputSize);

                auto sampleSize = AMediaExtractor_readSampleData(extractor, inputBuffer, inputSize);
                auto presentationTimeUs = AMediaExtractor_getSampleTime(extractor);

                if(sampleSize > 0) {
                    AMediaCodec_queueInputBuffer(codec, inIndex, 0, sampleSize, presentationTimeUs, 0);
                    AMediaExtractor_advance(extractor);
                } else {
                    AMediaCodec_queueInputBuffer(codec, inIndex, 0, 0, presentationTimeUs, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                    extractorDone = true;
                }
            } else if(inIndex != AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
                Log::error("AMediaCodec_dequeueInputBuffer failed: inIndex = ", inIndex);
                break;
            }

            AMediaCodecBufferInfo info;
            auto outIndex = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
            if(outIndex >= 0) {
                if(info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM)
                    codecDone = true;

                auto samplesRead = info.size / 2;
                std::size_t outBufferSize;
                std::uint8_t* outBuffer = AMediaCodec_getOutputBuffer(codec, outIndex, &outBufferSize);
                assert(curSample + samplesRead <= numSamples);
                oboe::convertPcm16ToFloat(reinterpret_cast<std::int16_t*>(outBuffer), &data[curSample], samplesRead);
                if(doubleSample) {
                    for(auto i = samplesRead - 1 ; i >= 0 ; i--) {
                        auto curr = data[curSample + i];
                        auto prev = i > 0 ? data[curSample + i - 1] :
                                     curSample > 0 ? data[curSample - 2] : 0.f;
                        data[curSample + 2 * i + 1] = curr;
                        data[curSample + 2 * i] = (curr + prev) / 2.f;
                    }
                    curSample += 2 * samplesRead;
                } else
                    curSample += samplesRead;
                AMediaCodec_releaseOutputBuffer(codec, outIndex, false);
                ret->m_samplesAvail.store(curSample, std::memory_order::release);
            } else
                switch(outIndex) {
                    case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED: {
                        auto ff = AMediaCodec_getOutputFormat(codec);
                        Log::verbose<Log::audio>("output format: ", AMediaFormat_toString(ff));
                        AMediaFormat_delete(ff);
                    }
                    case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
                    case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                        break;
                    default:
                        Log::error("AMediaCodec_dequeueOutputBuffer failed: outIndex = ", outIndex);
                        break;
                }
        } while(!(extractorDone && codecDone));
        Log::debug<Log::audio>("loadSoundAsync ", filename, ": decoded ", curSample, " frames");

        AMediaFormat_delete(format);
        AMediaCodec_delete(codec);
        AMediaExtractor_delete(extractor);
    }).detach();

    return ret;
}
