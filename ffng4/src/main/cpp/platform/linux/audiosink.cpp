#include "subsystem/audio.h"
#include <cerrno>
extern "C" {
#include <poll.h>
#include <alsa/asoundlib.h>
}

AudioSink::AudioSink(Audio& iface) : m_audio(iface), m_quit(false) {
    snd_pcm_t *alsa;
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_sw_params_t *sw_params;

    if(int err = snd_pcm_open(&alsa, "default", SND_PCM_STREAM_PLAYBACK, 0); err < 0)
        Log::fatal("cannot open audio device: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_malloc(&hw_params); err < 0)
        Log::fatal("snd_pcm_hw_params_malloc failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_any(alsa, hw_params); err < 0)
        Log::fatal("snd_pcm_hw_params_any failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_access(alsa, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED); err < 0)
        Log::fatal("snd_pcm_hw_params_set_access failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_format(alsa, hw_params, SND_PCM_FORMAT_FLOAT_LE); err < 0)
        Log::fatal("snd_pcm_hw_params_set_format failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_rate(alsa, hw_params, 22050, 0); err < 0)
        Log::fatal("snd_pcm_hw_params_set_rate failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_channels(alsa, hw_params, 1); err < 0)
        Log::fatal("snd_pcm_hw_params_set_channels failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_buffer_size(alsa, hw_params, bufSize); err < 0)
        Log::fatal("snd_pcm_hw_params_set_buffer_size failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params(alsa, hw_params); err < 0)
        Log::fatal("snd_pcm_hw_params failed: ", snd_strerror(err));

    snd_pcm_hw_params_free(hw_params);

    if(int err = snd_pcm_sw_params_malloc(&sw_params); err < 0)
        Log::fatal("snd_pcm_sw_params_malloc failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params_current(alsa, sw_params); err < 0)
        Log::fatal("snd_pcm_sw_params_current failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params_set_avail_min(alsa, sw_params, bufSize); err < 0)
        Log::fatal("snd_pcm_sw_params_set_avail_min failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params_set_start_threshold(alsa, sw_params, 0); err < 0)
        Log::fatal("snd_pcm_sw_params_set_start_threshold failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params(alsa, sw_params); err < 0)
        Log::fatal("snd_pcm_sw_params failed: ", snd_strerror(err));

    m_thread = std::thread([=, this]() {
        Log::info("Audio thread started.");

        if(int err = snd_pcm_prepare(alsa); err < 0)
            Log::fatal("snd_pcm_prepare failed: ", snd_strerror(err));

        snd_pcm_sframes_t numFrames;
        std::array<float, bufSize> buf;

        while(!m_quit.load(std::memory_order::relaxed)) {
            if(int err = snd_pcm_wait(alsa, -1); err < 0) {
                Log::error("snd_pcm_wait failed: ", strerror(errno));
                break;
            }

            numFrames = std::min(snd_pcm_avail_update(alsa), (snd_pcm_sframes_t)bufSize);
            if(numFrames < 0) {
                Log::error("snd_pcm_avail_update failed: ", snd_strerror(numFrames));
                break;
            }

            m_audio.mix(buf.data(), numFrames);
            if(int err = snd_pcm_writei(alsa, (void*)buf.data(), numFrames); err < 0)
                Log::error("snd_pcm_writei failed: ", snd_strerror(err));
        }

        Log::info("Audio thread exiting.");
        snd_pcm_close(alsa);
    });
}

AudioSink::~AudioSink() {
    m_quit.store(true, std::memory_order::relaxed);
    m_thread.join();
}

void AudioSink::start() {
}

void AudioSink::stop() {
}
