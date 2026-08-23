#pragma once

#include <string>
#include <utility>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pwnit::console
{

namespace detail
{

inline spdlog::logger &logger(const char *name)
{
    if (auto logger = spdlog::get(name))
        return *logger;

    return *spdlog::stdout_color_mt(name);
}

inline spdlog::logger &debug_logger()
{
    static auto &logger = detail::logger("pwnit.debug");

    static const bool init = [] {
        logger.set_pattern("[%H:%M:%S] [%^d%$] %v");
        logger.set_level(spdlog::level::trace);
        return true;
    }();

    (void)init;
    return logger;
}

inline spdlog::logger &info_logger()
{
    static auto &logger = detail::logger("pwnit.info");

    static const bool init = [] {
        logger.set_pattern("[%^i%$] %v");
        logger.set_level(spdlog::level::trace);
        return true;
    }();

    (void)init;
    return logger;
}

inline spdlog::logger &error_logger()
{
    static auto &logger = detail::logger("pwnit.error");

    static const bool init = [] {
        logger.set_pattern("%^%v%$");
        logger.set_level(spdlog::level::trace);
        return true;
    }();

    (void)init;
    return logger;
}

inline spdlog::logger &warn_logger()
{
    static auto &logger = detail::logger("pwnit.warn");

    static const bool init = [] {
        logger.set_pattern("%^%v%$");
        logger.set_level(spdlog::level::trace);
        return true;
    }();

    (void)init;
    return logger;
}

inline spdlog::logger &success_logger()
{
    static auto &logger = detail::logger("pwnit.success");

    static const bool init = [] {
        logger.set_pattern("%^%v%$");
        logger.set_level(spdlog::level::trace);
        return true;
    }();

    (void)init;
    return logger;
}

inline spdlog::logger &plain_logger()
{
    static auto &logger = detail::logger("pwnit.plain");

    static const bool init = [] {
        logger.set_pattern("%v");
        logger.set_level(spdlog::level::trace);
        return true;
    }();

    (void)init;
    return logger;
}

}

template <typename ...Args>
inline void debug(
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    SPDLOG_LOGGER_DEBUG(
        &detail::debug_logger(),
        fmt,
        std::forward<Args>(args)...
    );
}

template <typename ...Args>
inline void info(
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    SPDLOG_LOGGER_INFO(
        &detail::info_logger(),
        fmt,
        std::forward<Args>(args)...
    );
}

template <typename ...Args>
inline void error(
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    SPDLOG_LOGGER_ERROR(
        &detail::error_logger(),
        fmt,
        std::forward<Args>(args)...
    );
}

template <typename ...Args>
inline void warn(
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    SPDLOG_LOGGER_WARN(
        &detail::warn_logger(),
        fmt,
        std::forward<Args>(args)...
    );
}

template <typename ...Args>
inline void success(
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    SPDLOG_LOGGER_INFO(
        &detail::success_logger(),
        fmt,
        std::forward<Args>(args)...
    );
}

template <typename ...Args>
inline void log(
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    SPDLOG_LOGGER_INFO(
        &detail::plain_logger(),
        fmt,
        std::forward<Args>(args)...
    );
}

template <typename ...Args>
inline void message(
    const std::string &header,
    spdlog::format_string_t<Args...> fmt,
    Args &&...args
) {
    auto &logger = detail::logger(header.c_str());
    logger.set_pattern("%^%n%$ %v");

    logger.info(
        fmt::format(fmt, std::forward<Args>(args)...)
    );
}

}
