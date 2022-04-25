/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "Log.h"

#include <android/log.h>
#include <cstdarg>

//-----------------------------------------------------------------

void Log::log(int priority, const char *tag, const char *format, va_list va) {
    __android_log_vprint(priority, tag, format, va);
}

void Log::debug(const char *format, ...) {
    std::va_list ap;
    va_start(ap, format);
    log(ANDROID_LOG_DEBUG, tag, format, ap);
    va_end(ap);
}

void Log::info(const char *format, ...) {
    std::va_list ap;
    va_start(ap, format);
    log(ANDROID_LOG_INFO, tag, format, ap);
    va_end(ap);
}

void Log::warn(const char *format, ...) {
    std::va_list ap;
    va_start(ap, format);
    log(ANDROID_LOG_WARN, tag, format, ap);
    va_end(ap);
}

void Log::error(const char *format, ...) {
    std::va_list ap;
    va_start(ap, format);
    log(ANDROID_LOG_ERROR, tag, format, ap);
    va_end(ap);
}
