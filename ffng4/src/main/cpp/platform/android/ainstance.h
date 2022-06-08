#ifndef FISH_FILLETS_ANDROID_INSTANCE_H
#define FISH_FILLETS_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"

struct android_app;

struct PlatformInstance {
    android_app* app;
    jni::Env jni;

    PlatformInstance(Instance& instance, android_app* androidApp) : app(androidApp), jni(androidApp) { }

    void quit() {
        ANativeActivity_finish(app->activity);
    }
};

using PlatformData = android_app*;

template<>
inline Instance& Instance::get(android_app* app) {
    return *static_cast<Instance*>(app->userData);
}

#endif //FISH_FILLETS_ANDROID_INSTANCE_H
