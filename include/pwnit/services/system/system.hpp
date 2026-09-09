#pragma once

#include <format>
#include <optional>

#define OK 0
#define PROGRAM_ERROR 1
#define FORK_ERROR std::nullopt

namespace pwnit::system
{

template <typename ...Args>
std::optional<int> run(
    const std::format_string<Args...> &fmt, Args &&...args
) {
    const std::string cmd = std::format(fmt, std::forward<Args>(args)...);
    
    const int status = std::system(cmd.c_str());

    if (status == -1)
        return FORK_ERROR;

    if (!WIFEXITED(status))
        return FORK_ERROR;

    return WEXITSTATUS(status);
}
    
}
