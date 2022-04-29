#ifndef FILLETS_H
#define FILLETS_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cassert>
#include <stdexcept>

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <algorithm>

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

    [[maybe_unused]] void error(const std::string& desc) {
        error(desc.c_str());
    }
}

using namespace std::string_literals;

#endif //FILLETS_H