#include "log.h"
#include <android/log.h>

constexpr const char* tag = "Fillets";

static auto android_loglevel(Log::Level level) {
    switch(level) {
        case Log::Level::verbose:
            return ANDROID_LOG_VERBOSE;
        case Log::Level::debug:
            return ANDROID_LOG_DEBUG;
        case Log::Level::info:
            return ANDROID_LOG_INFO;
        case Log::Level::warn:
            return ANDROID_LOG_WARN;
        case Log::Level::error:
        case Log::Level::fatal:
            return ANDROID_LOG_ERROR;
    }
}

void Log::log(Level level, const std::string& message) {
    __android_log_print(android_loglevel(level), tag, "%s", message.c_str());
}
