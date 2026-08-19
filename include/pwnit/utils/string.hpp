#pragma once

#include <algorithm>
#include <format>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

namespace pwnit::string
{

template <std::size_t N>
struct fixed
{
    char data[N];

    constexpr fixed(const char (&str)[N])
    {
        std::copy_n(str, N, data);
    }

    constexpr operator std::string() const
    {
        return {data, N - 1};
    }

    constexpr operator std::string_view() const
    {
        return {data, N - 1};
    }

    constexpr std::string_view view() const noexcept
    {
        return {data, N - 1};
    }
};

template <std::size_t N, std::size_t M>
constexpr auto operator+(const fixed<N>& lhs, const fixed<M>& rhs)
    -> std::string
{
    std::string result;
    result.reserve((N - 1) + (M - 1));
    result.append(lhs.data, N - 1);
    result.append(rhs.data, M - 1);
    return result;
}

template <std::size_t N>
constexpr auto operator+(const fixed<N>& lhs, std::string_view rhs)
    -> std::string
{
    std::string result;
    result.reserve((N - 1) + rhs.size());
    result.append(lhs.data, N - 1);
    result.append(rhs);
    return result;
}

template <std::size_t N>
constexpr auto operator+(std::string_view lhs, const fixed<N>& rhs)
    -> std::string
{
    std::string result;
    result.reserve(lhs.size() + (N - 1));
    result.append(lhs);
    result.append(rhs.data, N - 1);
    return result;
}

template <std::size_t N>
constexpr auto operator+(const fixed<N>& lhs, const char* rhs)
    -> std::string
{
    return std::string{lhs.view()} + rhs;
}

template <std::size_t N>
constexpr auto operator+(const char* lhs, const fixed<N>& rhs)
    -> std::string
{
    return std::string{lhs} + rhs.view();
}

template <std::size_t N>
constexpr auto operator+(const fixed<N>& lhs, const std::string& rhs)
    -> std::string
{
    return std::string{lhs.view()} + rhs;
}

template <std::size_t N>
constexpr auto operator+(const std::string& lhs, const fixed<N>& rhs)
    -> std::string
{
    return lhs + std::string{rhs.view()};
}

}

template <std::size_t N>
struct fmt::formatter<pwnit::string::fixed<N>>
    : fmt::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(
        const pwnit::string::fixed<N>& value,
        FormatContext& ctx
    ) const
    {
        return fmt::formatter<std::string_view>::format(
            value.view(),
            ctx
        );
    }
};

template <std::size_t N>
struct std::formatter<pwnit::string::fixed<N>, char>
    : std::formatter<std::string_view, char>
{
    constexpr auto format(
        const pwnit::string::fixed<N>& value,
        std::format_context& ctx
    ) const
    {
        return std::formatter<std::string_view, char>::format(
            value.view(),
            ctx
        );
    }
};
