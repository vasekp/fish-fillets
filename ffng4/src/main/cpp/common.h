#ifndef FILLETS_H
#define FILLETS_H

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <initializer_list>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include <cerrno>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "FFNG4", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "FFNG4", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "FFNG4", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "FFNG4", __VA_ARGS__))

struct saved_state {
    float angle;
    float x;
    float y;
};

struct Instance {
    struct android_app* app;
    JNIEnv* jni;
    bool animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    std::int32_t width;
    std::int32_t height;
    struct saved_state state;
};

using namespace std::string_literals;

#endif //FILLETS_H