#ifndef FILLETS_H
#define FILLETS_H

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <initializer_list>

#include <stdexcept>
#include <cassert>

#include <android/log.h>
#include <android_native_app_glue.h>

#include "ndk.h"

#define APP_TAG "FFNG4"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, APP_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, APP_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, APP_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, APP_TAG, __VA_ARGS__))

struct android_app;
struct Display;
struct Image;

struct saved_state {
    float angle;
    float x;
    float y;
};

struct Instance {
    android_app* app;
    ndk::JNIEnv jni;

    std::unique_ptr<Display> display;
    std::unique_ptr<Image> bg;

    struct saved_state state;
    bool animating;

    Instance(android_app* _app) :
        app(_app),
        jni(app->activity->vm)
    { }
};

using namespace std::string_literals;

#endif //FILLETS_H