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

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <jni.h>
#include <android/log.h>

#define APP_TAG "FFNG4"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, APP_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, APP_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, APP_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, APP_TAG, __VA_ARGS__))

struct saved_state {
    float angle;
    float x;
    float y;
};

struct Image;

struct Instance {
    struct android_app* app;
    JNIEnv* jni;
    bool animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    std::unique_ptr<Image> bg;
    std::int32_t width;
    std::int32_t height;
    struct saved_state state;
};

using namespace std::string_literals;

#endif //FILLETS_H