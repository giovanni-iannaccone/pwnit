#pragma once

#include <cstdlib>

#include <pwnit/utils/console.hpp>

#include <spdlog/spdlog.h>
#include <utility>

namespace pwnit::assert
{
    template <typename ...Args>
    constexpr bool check(bool cond, spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        if (cond)
            return true;

        console::error(fmt, std::forward<Args>(args)...);
        return false;
    }

    template <typename ...Args>
    constexpr void fail(bool cond, spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        if (!check(cond, fmt, std::forward<Args>(args)...))
            std::exit(EXIT_FAILURE);        
    }
};
