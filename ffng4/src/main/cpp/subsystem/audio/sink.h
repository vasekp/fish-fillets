#ifndef FF_AUDIO_SINK_H
#define FF_AUDIO_SINK_H

class Audio;

class AudioSink {
protected:
    Audio& m_audio;
    AudioSink(Audio& iface) : m_audio(iface) { }
    virtual ~AudioSink() { }

public:
    virtual void start() = 0;
    virtual void stop() = 0;
};

#endif //FF_AUDIO_SINK_H