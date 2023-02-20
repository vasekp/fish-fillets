#ifndef FISH_FILLETS_OBOESINK_H
#define FISH_FILLETS_OBOESINK_H

#include "instance.h"
#include "oboestream.h"

class OboeSink: public oboe::AudioStreamErrorCallback {
    Instance& m_instance;
    std::unique_ptr<OboeStream> m_stream;
    bool m_running;

public:
    OboeSink(Instance& instance) : m_instance(instance), m_stream(), m_running(false) { }

    void open() {
        m_stream = std::make_unique<OboeStream>(m_instance.audio(), this);
    }

    void start() {
        m_running = true;
        m_stream->start();
    }

    void stop() {
        m_stream->stop();
        m_running = false;
    }

    void close() {
        stop();
        m_stream.reset();
    }

private:
    void onErrorAfterClose(oboe::AudioStream* stream, oboe::Result error) override {
        if(error == oboe::Result::ErrorDisconnected) {
            Log::info("restarting Oboe stream");
            open();
            if(m_running)
                start();
        } else
            Log::error("Oboe error: ", (int)error);
    }
};

#endif