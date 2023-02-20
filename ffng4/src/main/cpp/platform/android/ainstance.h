#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "./files.h"
#include "./input.h"
#include "./oboesink.h"

struct android_app;

class AndroidInstance : public Instance, public oboe::AudioStreamErrorCallback {
    std::unique_ptr<OboeSink> m_sink{};
    AndroidFiles m_files;
    AndroidInput m_input;
    bool m_audioOn;

public:
    android_app* app;
    jni::Env jni;

    AndroidInstance(android_app* androidApp) : m_files(androidApp), m_input(*this), app(androidApp), jni(androidApp) {
        app->userData = this;
        init();
    }

    static AndroidInstance& get(android_app* app) {
        return *static_cast<AndroidInstance*>(app->userData);
    }

    void openAudio() {
        m_sink = std::make_unique<OboeSink>(audio(), this);
    }

    void startAudio() {
        m_audioOn = true;
        m_sink->start();
    }

    void stopAudio() {
        m_sink->stop();
        m_audioOn = false;
    }

    void closeAudio() {
        m_sink.reset();
    }

    void quit() override {
        Instance::quit();
        ANativeActivity_finish(app->activity);
    }

    void* window() override {
        return app->window;
    }

    AndroidFiles& files() override {
        return m_files;
    }

    AndroidInput& inputSource() override {
        return m_input;
    }

private:
    void onErrorAfterClose(oboe::AudioStream* stream, oboe::Result error) override {
        if(error == oboe::Result::ErrorDisconnected) {
            Log::info("restarting Oboe stream");
            m_sink = std::make_unique<OboeSink>(audio(), this);
            if(m_audioOn)
                startAudio();
        } else {
            Log::info("Oboe error: ", (int)error);
        }
    }
};

#endif //FF_ANDROID_INSTANCE_H
