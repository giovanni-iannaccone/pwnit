#pragma once

#include <cstdlib>

#include <fstream>
#include <pwnit/console/console.hpp>

#include <spdlog/spdlog.h>

namespace pwnit::utils
{
    template <typename ...Args>
    constexpr void assert_fail(bool cond, spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        if (cond) return;

        console::error(fmt, std::forward<Args>(args)...);
        std::exit(EXIT_FAILURE);
    }

    inline std::string read_whole_file(std::ifstream &file)
    {
        return {
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };
    }
};
