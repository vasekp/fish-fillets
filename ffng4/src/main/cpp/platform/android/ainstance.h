#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "./files.h"
#include "./oboesink.h"

struct android_app;

class AndroidInstance : public Instance {
    std::unique_ptr<OboeSink> sink{};

public:
    android_app* app;
    jni::Env jni;

    AndroidInstance(android_app* androidApp) : Instance(std::make_unique<AndroidFiles>(androidApp)), app(androidApp), jni(androidApp) {
        app->userData = this;
    }

    static AndroidInstance& get(android_app* app) {
        return *static_cast<AndroidInstance*>(app->userData);
    }

    OboeSink* openAudio() {
        sink = std::make_unique<OboeSink>(audio());
        return sink.get();
    }

    void closeAudio() {
        sink.reset();
    }

    void quit() override {
        Instance::quit();
        ANativeActivity_finish(app->activity);
    }

    void* window() override {
        return app->window;
    }
};

#endif //FF_ANDROID_INSTANCE_H
