#include "log.h"
#include <iostream>

static auto prefix(Log::Level level) {
    switch(level) {
        case Log::Level::verbose:
            return 'V';
        case Log::Level::debug:
            return 'D';
        case Log::Level::info:
            return 'I';
        case Log::Level::warn:
            return 'W';
        case Log::Level::error:
        case Log::Level::fatal:
            return 'E';
    }
    std::unreachable();
}

void Log::log(Level level, const std::string& message) {
    std::ostringstream oss;
    oss << prefix(level) << ": " << message << '\n';
    std::cerr << oss.str();
}
