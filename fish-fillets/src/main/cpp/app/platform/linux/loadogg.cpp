#include "subsystem/audio/audiodata.h"
#include "subsystem/files.h"
#include "files.h"
#include <cstdio>
#include <cstring>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <thread>

static AudioData::Ref loadSoundAsync(Instance& instance, const std::string& filename);

namespace decoders {
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
    auto path = dynamic_cast<SystemFile&>(*instance.files().system(filename)).fullPath();
    OggVorbis_File vf;
    bool doubleSample;
    std::size_t numSamples;

    if(ov_fopen(path.c_str(), &vf)) {
        Log::error("ov_fopen failed on ", path);
        return {};
    }

    {
        vorbis_info* vi = ov_info(&vf, -1);
        Log::verbose<Log::audio>(filename, ": sample rate ", vi->rate, ", ", vi->channels, " channels");

        if(vi->channels != 1) {
            Log::error("Unexpected channel count (", filename, "), expected mono: ", vi->channels);
            return {};
        }

        switch(vi->rate) {
            case 22050:
                doubleSample = false;
                break;
            case 11025:
                doubleSample = true;
                break;
            default:
                Log::error("Unexpected audio sample rate (", filename, "): ", vi->rate);
                return {};
        }

        numSamples = (std::size_t)ov_pcm_total(&vf, -1);
        if(doubleSample)
            numSamples *= 2;
        Log::verbose<Log::audio>("numSamples: ", numSamples);
    }

    auto ref = AudioData::create(filename, numSamples);
    std::thread([=] () mutable {
        auto data = ref->data();
        std::size_t curSample = 0;
        while(true) {
            float** buffer;
            int section;
            if(auto ret = ov_read_float(&vf, &buffer, 1, &section); ret > 0) {
                auto samplesRead = (std::size_t)ret;
                if(doubleSample) {
                    assert(curSample + 2u * samplesRead <= numSamples);
                    for(auto i = 0u; i < samplesRead; i++) {
                        float curr = buffer[0][i];
                        float prev = i > 0 ? buffer[0][i - 1] :
                                     curSample > 0 ? data[curSample - 1] : 0.f;
                        data[curSample + 2 * i + 1] = curr;
                        data[curSample + 2 * i] = (curr + prev) / 2.f;
                    }
                    curSample += 2u * samplesRead;
                } else {
                    std::memcpy(&data[curSample], buffer[0], samplesRead * sizeof(float));
                    curSample += samplesRead;
                }
                ref->m_samplesAvail.store(curSample, std::memory_order::release);
            } else if(ret == 0)
                break;
            else {
                Log::error("Error in decoding Vorbis data (", filename, "): ", ret);
                break;
            }
        }
        Log::debug<Log::audio>("loadSoundAsync ", filename, ": decoded ", curSample, " frames");
        ov_clear(&vf);
    }).detach();

    return ref;
}
