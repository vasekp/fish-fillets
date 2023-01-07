#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "./files.h"
#include "./input.h"
#include "./oboesink.h"

struct android_app;

class AndroidInstance : public Instance {
    std::unique_ptr<OboeSink> m_sink{};
    AndroidInput m_input;

public:
    android_app* app;
    jni::Env jni;

    AndroidInstance(android_app* androidApp) : Instance(std::make_unique<AndroidFiles>(androidApp)), m_input(*this), app(androidApp), jni(androidApp) {
        app->userData = this;
    }

    static AndroidInstance& get(android_app* app) {
        return *static_cast<AndroidInstance*>(app->userData);
    }

    OboeSink* openAudio() {
        m_sink = std::make_unique<OboeSink>(audio());
        return m_sink.get();
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

    AndroidInput& inputSource() override {
        return m_input;
    }
};

#endif //FF_ANDROID_INSTANCE_H
