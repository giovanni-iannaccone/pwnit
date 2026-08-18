#pragma once

#include <spdlog/spdlog.h>

namespace pwnit::console
{
    template <typename ...Args>
    void debug(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        spdlog::set_pattern("[%H:%M:%S] [%^d%$] %v");
        spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void error(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        spdlog::set_pattern("%^%v%$");
        spdlog::error(fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void info(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        spdlog::set_pattern("[%^i%$] %v");
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void log(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        spdlog::set_pattern("%v");
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void success(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        spdlog::set_pattern("%^%v%$");
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void warn(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        spdlog::set_pattern("%^%v%$");
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }
}
