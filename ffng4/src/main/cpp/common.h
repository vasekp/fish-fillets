#ifndef FILLETS_H
#define FILLETS_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <initializer_list>

#include <stdexcept>
#include <cassert>

#include <android/log.h>

#define APP_TAG "FFNG4"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, APP_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, APP_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, APP_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, APP_TAG, __VA_ARGS__))

namespace {
    [[maybe_unused]] void error(const char *desc, const char *format, ...) {
        std::va_list ap;
        va_start(ap, format);
        __android_log_vprint(ANDROID_LOG_ERROR, APP_TAG, format, ap);
        va_end(ap);
        throw std::runtime_error(desc);
    }

    [[maybe_unused]] void error(const char *desc) {
        error(desc, "%s", desc);
    }
}

using namespace std::string_literals;

#endif //FILLETS_H