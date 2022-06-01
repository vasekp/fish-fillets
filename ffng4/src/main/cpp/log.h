#ifndef FISH_FILLETS_LOG_H
#define FISH_FILLETS_LOG_H

#include <sstream>
#include <string_view>
#include <stdexcept>

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

    template<typename... Ts>
    static void verbose(const Ts& ... ts) {
        log(Level::verbose, format(ts...));
    }

    template<typename... Ts>
    static void debug(const Ts& ... ts) {
        log(Level::debug, format(ts...));
    }

    template<typename... Ts>
    static void info(const Ts& ... ts) {
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
};

#endif
