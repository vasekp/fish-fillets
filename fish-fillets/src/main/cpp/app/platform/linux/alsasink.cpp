#include "alsasink.h"
#include <cerrno>
extern "C" {
#include <poll.h>
#include <alsa/asoundlib.h>
}

AlsaSink::AlsaSink(Audio& iface) : m_audio(iface), m_pause(false), m_quit(false) {
    snd_pcm_t* alsa;
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_sw_params_t* sw_params;

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

    unsigned int num, den;
    snd_pcm_hw_params_get_rate_numden(hw_params, &num, &den);
    Log::debug<Log::audio>("numden: ", num, " ", den);

    if(int err = snd_pcm_hw_params_set_channels(alsa, hw_params, 1); err < 0)
        Log::fatal("snd_pcm_hw_params_set_channels failed: ", snd_strerror(err));

    auto bufSize = bufSizeTarget;
    if(int err = snd_pcm_hw_params_set_buffer_size_near(alsa, hw_params, &bufSize); err < 0)
        Log::fatal("snd_pcm_hw_params_set_buffer_size_near failed: ", snd_strerror(err));
    Log::debug<Log::audio>("Alsa buffer size: ", bufSize);

    auto periodSize = bufSize / 2;
    if(int err = snd_pcm_hw_params_set_period_size_near(alsa, hw_params, &periodSize, 0); err < 0)
        Log::fatal("snd_pcm_hw_params_set_period_size_near failed: ", snd_strerror(err));
    Log::debug<Log::audio>("Alsa period size: ", periodSize);

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

    std::unique_lock lock(m_mutex);
    m_pause.store(true, std::memory_order::release);
    m_thread = std::thread([=, this]() {
        Log::debug<Log::audio>("Audio thread started.");
        m_pause.store(false, std::memory_order::release);
        m_cond.notify_one();

        if(int err = snd_pcm_prepare(alsa); err < 0)
            Log::fatal("snd_pcm_prepare failed: ", snd_strerror(err));

        auto buffer = std::make_unique<float[]>(bufSize);
        snd_pcm_sframes_t numFrames;

        while(!m_quit.load(std::memory_order::relaxed)) {
            if(m_pause.load(std::memory_order::relaxed)) {
                Log::debug<Log::audio>("Audio: pausing");
                snd_pcm_pause(alsa, 1);
                std::unique_lock lock(m_mutex);
                m_cond.wait(lock, [this] { return !m_pause.load(std::memory_order::relaxed) || m_quit.load(std::memory_order::relaxed); });
                Log::debug<Log::audio>("Audio: resuming");
                snd_pcm_pause(alsa, 0);
            }

            if(int err = snd_pcm_wait(alsa, millisRefresh); err < 0) {
                Log::error("snd_pcm_wait failed: ", strerror(errno));
                break;
            }

            numFrames = std::min(snd_pcm_avail_update(alsa), (snd_pcm_sframes_t)bufSize);
            if(numFrames < 0) {
                Log::error("snd_pcm_avail_update failed: ", snd_strerror(numFrames));
                break;
            }

            m_audio.mix(buffer.get(), numFrames);
            if(int err = snd_pcm_writei(alsa, (void*)buffer.get(), numFrames); err < 0)
                Log::error("snd_pcm_writei failed: ", snd_strerror(err));
        }

        Log::debug<Log::audio>("Audio thread exiting.");
        snd_pcm_close(alsa);
    });
    m_cond.wait(lock, [this] { return !m_pause.load(std::memory_order::acquire); });
}

AlsaSink::~AlsaSink() {
    m_quit.store(true, std::memory_order::relaxed);
    m_cond.notify_one();
    m_thread.join();
}

void AlsaSink::pause() {
    m_pause.store(true, std::memory_order::relaxed);
}

void AlsaSink::resume() {
    m_pause.store(false, std::memory_order::relaxed);
    m_cond.notify_one();
}
