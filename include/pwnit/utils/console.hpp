#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fmt/format.h>
#include <mutex>
#include <string>
#include <string_view>
#include <sstream>
#include <utility>

namespace pwnit::console
{

namespace detail
{

inline constexpr std::string_view reset   = "\033[0m";
inline constexpr std::string_view cyan    = "\033[36m";
inline constexpr std::string_view green   = "\033[32m";
inline constexpr std::string_view yellow  = "\033[33m";
inline constexpr std::string_view red     = "\033[31m";
inline constexpr std::string_view magenta = "\033[35m";

inline std::mutex &mutex()
{
    static std::mutex m;
    return m;
}

inline std::string timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};

#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream out;
    out << std::put_time(&tm, "%H:%M:%S");

    return out.str();
}

inline void write(std::string_view text)
{
    std::lock_guard<std::mutex> lock(mutex());

    std::cout.write(
        text.data(),
        static_cast<std::streamsize>(text.size())
    );

    std::cout.flush();
}

inline void colored(
    std::string_view color,
    std::string_view text
)
{
    std::string output;

    output.reserve(
        color.size() +
        text.size() +
        reset.size() + 1
    );

    output.append(color);
    output.append(text);
    output.append(reset);
    output.push_back('\n');

    write(output);
}

}

template<typename... Args>
inline void debug(
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::colored(
        detail::cyan,
        fmt::format(
            "[{}] [d] {}",
            detail::timestamp(),
            fmt::format(fmt, std::forward<Args>(args)...)
        )
    );
}

template<typename... Args>
inline void info(
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::colored(
        detail::green,
        fmt::format(fmt, std::forward<Args>(args)...)
    );
}

template<typename... Args>
inline void success(
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::colored(
        detail::green,
        fmt::format(fmt, std::forward<Args>(args)...)
    );
}

template<typename... Args>
inline void warn(
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::colored(
        detail::yellow,
        fmt::format(fmt, std::forward<Args>(args)...)
    );
}

template<typename... Args>
inline void log(
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::write(
        fmt::format(fmt, std::forward<Args>(args)...) + "\n"
    );
}
template<typename... Args>
inline void error(
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::colored(
        detail::red,
        fmt::format(fmt, std::forward<Args>(args)...)
    );
}

template<typename... Args>
inline void message(
    const std::string& header,
    fmt::format_string<Args...> fmt,
    Args&&... args
)
{
    std::string output;

    output += detail::green;
    output += header;
    output += detail::reset;
    output += " ";
    output += fmt::format(fmt, std::forward<Args>(args)...);
    output += "\n";

    detail::write(output);
}

}
