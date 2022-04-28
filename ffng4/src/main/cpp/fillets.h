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
#include <jni.h>
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

#endif //FILLETS_H