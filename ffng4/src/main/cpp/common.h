#ifndef FIST_FILLETS_COMMON_H
#define FIST_FILLETS_COMMON_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cassert>
#include <stdexcept>
#include <cmath>

#include <string>
#include <string_view>
#include <vector>
#include <bitset>
#include <deque>
#include <array>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <memory>
#include <algorithm>
#include <chrono>
#include <optional>

#include <android/log.h>

#include "types.h"

#define APP_TAG "FFNG4"

#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, APP_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, APP_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, APP_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, APP_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, APP_TAG, __VA_ARGS__))

namespace {
    [[maybe_unused, noreturn]] void error(const char *desc, const char *format, ...) {
        std::va_list ap;
        va_start(ap, format);
        __android_log_vprint(ANDROID_LOG_ERROR, APP_TAG, format, ap);
        va_end(ap);
        throw std::runtime_error(desc);
    }

    [[maybe_unused, noreturn]] void error(const char *desc) {
        error(desc, "%s", desc);
    }

    [[maybe_unused, noreturn]] void error(const std::string& desc) {
        error(desc.c_str());
    }
}

using namespace std::string_literals;

#endif //FIST_FILLETS_COMMON_H