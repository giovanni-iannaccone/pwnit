#pragma once

#include <format>
#include <optional>

namespace pwnit::system
{

template <typename ...Args>
std::optional<int> run(
    const std::format_string<Args...> &fmt, Args &&...args
) {
    const std::string cmd = std::format(fmt, std::forward<Args>(args)...);
    
    const int status = std::system(cmd.c_str());

    if (status == -1)
        return std::nullopt;

    if (!WIFEXITED(status))
        return std::nullopt;

    return WEXITSTATUS(status);
}
    
}
