#include "log.h"
#include <os/log.h>

static auto ios_loglevel(Log::Level level) {
    switch(level) {
        case Log::Level::verbose:
        case Log::Level::debug:
            return OS_LOG_TYPE_DEBUG;
        case Log::Level::info:
            return OS_LOG_TYPE_INFO;
        case Log::Level::warn:
        case Log::Level::error:
        case Log::Level::fatal:
            return OS_LOG_TYPE_ERROR;
        default:
            std::unreachable();
    }
}

void Log::log(Level level, const std::string& message) {
    os_log_with_type(OS_LOG_DEFAULT, ios_loglevel(level), "%{public}s", message.c_str());
}
