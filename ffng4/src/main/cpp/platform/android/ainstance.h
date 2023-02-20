#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "./files.h"
#include "./input.h"
#include "./oboesink.h"

struct android_app;

class AndroidInstance : public Instance {
    AndroidFiles m_files;
    AndroidInput m_input;
    OboeSink m_sink;

public:
    android_app* app;
    jni::Env jni;

    AndroidInstance(android_app* androidApp) : m_files(androidApp), m_input(*this), m_sink(*this), app(androidApp), jni(androidApp) {
        app->userData = this;
        init();
    }

    static AndroidInstance& get(android_app* app) {
        return *static_cast<AndroidInstance*>(app->userData);
    }

    void quit() override {
        Instance::quit();
        ANativeActivity_finish(app->activity);
    }

    void* window() override {
        return app->window;
    }

    OboeSink& oboe() {
        return m_sink;
    }

    AndroidFiles& files() override {
        return m_files;
    }

    AndroidInput& inputSource() override {
        return m_input;
    }
};

#endif //FF_ANDROID_INSTANCE_H
