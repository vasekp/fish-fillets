#include "alsasink.h"
#include "subsystem/audio.h"
#include <cerrno>
#include <poll.h>

AlsaSink::AlsaSink(Audio& iface) : m_audio(iface), m_pause(false), m_quit(false) {
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_sw_params_t* sw_params;

    if(int err = snd_pcm_open(&m_pcm, "default", SND_PCM_STREAM_PLAYBACK, 0); err < 0)
        Log::fatal("cannot open audio device: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_malloc(&hw_params); err < 0)
        Log::fatal("snd_pcm_hw_params_malloc failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_any(m_pcm, hw_params); err < 0)
        Log::fatal("snd_pcm_hw_params_any failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_access(m_pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED); err < 0)
        Log::fatal("snd_pcm_hw_params_set_access failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_format(m_pcm, hw_params, SND_PCM_FORMAT_FLOAT_LE); err < 0)
        Log::fatal("snd_pcm_hw_params_set_format failed: ", snd_strerror(err));

    if(int err = snd_pcm_hw_params_set_rate(m_pcm, hw_params, 22050, 0); err < 0)
        Log::fatal("snd_pcm_hw_params_set_rate failed: ", snd_strerror(err));

    unsigned int num, den;
    snd_pcm_hw_params_get_rate_numden(hw_params, &num, &den);
    Log::debug<Log::audio>("numden: ", num, " ", den);

    if(int err = snd_pcm_hw_params_set_channels(m_pcm, hw_params, 1); err < 0)
        Log::fatal("snd_pcm_hw_params_set_channels failed: ", snd_strerror(err));

    m_bufSize = bufSizeTarget;
    if(int err = snd_pcm_hw_params_set_buffer_size_near(m_pcm, hw_params, &m_bufSize); err < 0)
        Log::fatal("snd_pcm_hw_params_set_buffer_size_near failed: ", snd_strerror(err));
    Log::debug<Log::audio>("Alsa buffer size: ", m_bufSize);

    auto periodSize = m_bufSize / 2;
    if(int err = snd_pcm_hw_params_set_period_size_near(m_pcm, hw_params, &periodSize, 0); err < 0)
        Log::fatal("snd_pcm_hw_params_set_period_size_near failed: ", snd_strerror(err));
    Log::debug<Log::audio>("Alsa period size: ", periodSize);

    if(int err = snd_pcm_hw_params(m_pcm, hw_params); err < 0)
        Log::fatal("snd_pcm_hw_params failed: ", snd_strerror(err));

    snd_pcm_hw_params_free(hw_params);

    if(int err = snd_pcm_sw_params_malloc(&sw_params); err < 0)
        Log::fatal("snd_pcm_sw_params_malloc failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params_current(m_pcm, sw_params); err < 0)
        Log::fatal("snd_pcm_sw_params_current failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params_set_avail_min(m_pcm, sw_params, m_bufSize); err < 0)
        Log::fatal("snd_pcm_sw_params_set_avail_min failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params_set_start_threshold(m_pcm, sw_params, 0); err < 0)
        Log::fatal("snd_pcm_sw_params_set_start_threshold failed: ", snd_strerror(err));

    if(int err = snd_pcm_sw_params(m_pcm, sw_params); err < 0)
        Log::fatal("snd_pcm_sw_params failed: ", snd_strerror(err));

    std::unique_lock lock(m_mutex);
    m_pause.store(true, std::memory_order::release);
    m_thread = std::thread([this] { worker(); });
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

void AlsaSink::worker() {
    Log::debug<Log::audio>("Audio thread started.");
    m_pause.store(false, std::memory_order::release);
    m_cond.notify_one();

    if(int err = snd_pcm_prepare(m_pcm); err < 0)
        Log::fatal("snd_pcm_prepare failed: ", snd_strerror(err));

    auto buffer = std::make_unique<float[]>(m_bufSize);
    snd_pcm_sframes_t numFrames;

    while(!m_quit.load(std::memory_order::relaxed)) {
        if(m_pause.load(std::memory_order::relaxed)) {
            Log::debug<Log::audio>("Audio: pausing");
            snd_pcm_pause(m_pcm, 1);
            std::unique_lock lock(m_mutex);
            m_cond.wait(lock, [this] { return !m_pause.load(std::memory_order::relaxed) || m_quit.load(std::memory_order::relaxed); });
            Log::debug<Log::audio>("Audio: resuming");
            snd_pcm_pause(m_pcm, 0);
        }

        if(int err = snd_pcm_wait(m_pcm, millisRefresh); err < 0) {
            if(tryRestart(err))
                continue;
            else {
                Log::error("snd_pcm_wait failed: ", snd_strerror(err));
                break;
            }
        }

        numFrames = std::min(snd_pcm_avail_update(m_pcm), (snd_pcm_sframes_t)m_bufSize);
        if(numFrames < 0) {
            if(tryRestart(numFrames))
                continue;
            else {
                Log::error("snd_pcm_avail_update failed: ", snd_strerror(numFrames));
                break;
            }
        }

        m_audio.mix(buffer.get(), numFrames);
        if(int err = snd_pcm_writei(m_pcm, (void*)buffer.get(), numFrames); err < 0) {
            if(tryRestart(err))
                continue;
            else {
                Log::error("snd_pcm_writei failed: ", snd_strerror(err));
                break;
            }
        }
    }

    Log::debug<Log::audio>("Audio thread exiting.");
    snd_pcm_close(m_pcm);
}

bool AlsaSink::tryRestart(int err) {
    if(err == -EPIPE) {
        Log::debug<Log::audio>("Audio underrun, trying to restart");
        if(err = snd_pcm_prepare(m_pcm); err == 0) {
            Log::debug<Log::audio>("snd_pcm_prepare successful");
            return true;
        } else {
            Log::debug<Log::audio>("snd_pcm_prepare failed: ", snd_strerror(err));
            return false;
        }
    } else if(err == -ESTRPIPE) {
        while((err = snd_pcm_resume(m_pcm)) == -EAGAIN)
            std::this_thread::sleep_for(100ms);
        if(err == 0) {
            Log::debug<Log::audio>("snd_pcm_resume successful");
            return true;
        } else if(err = snd_pcm_prepare(m_pcm); err == 0) {
            Log::debug<Log::audio>("snd_pcm_prepare successful");
            return true;
        } else {
            Log::debug<Log::audio>("snd_pcm_prepare failed: ", snd_strerror(err));
            return false;
        }
    } else
        return false;
}
