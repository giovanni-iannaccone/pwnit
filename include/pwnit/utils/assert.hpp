#pragma once

#include <cstdlib>

#include <pwnit/utils/console.hpp>

#include <spdlog/spdlog.h>

namespace pwnit::assert
{
    template <typename ...Args>
    constexpr void fail(bool cond, spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        if (cond) return;

        console::error(fmt, std::forward<Args>(args)...);
        std::exit(EXIT_FAILURE);
    }
};
