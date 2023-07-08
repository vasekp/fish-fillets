#ifndef FISH_FILLETS_LOG_H
#define FISH_FILLETS_LOG_H

#include "common.h"
#include <sstream>

class Log {
public:
    enum class Level {
        verbose,
        debug,
        info,
        warn,
        error,
        fatal
    };

    template<Level minLevel, typename... Ts>
    static void verbose([[maybe_unused]] const Ts& ... ts) {
        if constexpr((int)minLevel <= (int)Level::verbose)
            log(Level::verbose, format(ts...));
    }

    template<Level minLevel, typename... Ts>
    static void debug([[maybe_unused]] const Ts& ... ts) {
        if constexpr((int)minLevel <= (int)Level::debug)
            log(Level::debug, format(ts...));
    }

    template<Level minLevel, typename... Ts>
    static void info([[maybe_unused]] const Ts& ... ts) {
        if constexpr((int)minLevel <= (int)Level::info)
            log(Level::info, format(ts...));
    }

    template<typename... Ts>
    static void warn(const Ts& ... ts) {
        log(Level::warn, format(ts...));
    }

    template<typename... Ts>
    static void error(const Ts& ... ts) {
        log(Level::error, format(ts...));
    }

    template<typename... Ts>
    [[noreturn]] static void fatal(const Ts& ... ts) {
        const std::string msg = format(ts...);
        log(Level::fatal, msg);
        throw std::runtime_error(msg);
    }

private:
    template<typename... Ts>
    static std::string format(const Ts& ... ts) {
        std::ostringstream oss;
        (oss << ... << ts);
        return oss.str();
    }

    static void log(Level level, const std::string& message);

public:
    static constexpr inline Level video = Level::error;
    static constexpr inline Level strings = Level::error;
    static constexpr inline Level audio = Level::error;
    static constexpr inline Level input = Level::error;
    static constexpr inline Level motion = Level::error;
    static constexpr inline Level gotos = Level::error;
    static constexpr inline Level lua = Level::error;
    static constexpr inline Level graphics = Level::error;
    static constexpr inline Level lifecycle = Level::error;
    static constexpr inline Level platform = Level::error;
    static constexpr inline Level persist = Level::error;
};

#endif
