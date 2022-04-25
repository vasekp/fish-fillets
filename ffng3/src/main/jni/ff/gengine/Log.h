#ifndef HEADER_LOG_H
#define HEADER_LOG_H

#include "android/log.h"

class Log {
public:
    static void debug(const char* format, ...);
    static void info(const char* format, ...);
    static void warn(const char* format, ...);
    static void error(const char* format, ...);

private:
    static void log(int priority, const char* tag, const char* format, va_list va);

    constexpr static const char* tag = "FFNG";
};

#endif
