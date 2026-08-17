#pragma once

#include <cstdint>

namespace pwnit::elf
{
    constexpr auto CANARY = 0;
    constexpr auto NX = 1;
    constexpr auto PIE = 2;
    constexpr auto RELRO_ = 3;

    constexpr auto STATICAL = 5;
    constexpr auto STRIPPED = 6;
    
    constexpr uint8_t GET_CANARY(uint8_t value) noexcept
    {
        return value & (1 << CANARY);
    }

    constexpr uint8_t SET_CANARY(uint8_t value) noexcept
    {
        return static_cast<uint8_t>(value << CANARY);
    }

    constexpr uint8_t GET_PIE(uint8_t value) noexcept
    {
        return value & (1 << PIE);
    }

    constexpr uint8_t SET_PIE(uint8_t value) noexcept
    {
        return static_cast<uint8_t>(value << PIE);
    }

    constexpr uint8_t GET_NX(uint8_t value) noexcept
    {
        return value & (1 << NX);
    }
    
    constexpr uint8_t SET_NX(uint8_t value) noexcept
    {
        return static_cast<uint8_t>(value << NX);
    }

    constexpr uint8_t GET_RELRO(uint8_t value) noexcept
    {
        return (value >> RELRO_) & 0b11u;
    }

    constexpr uint8_t SET_RELRO(uint8_t value) noexcept
    {
        return static_cast<uint8_t>(value << RELRO_);
    }   

    constexpr uint8_t GET_STATICAL(uint8_t value) noexcept
    {
        return value & (1 << STATICAL);
    }

    constexpr uint8_t SET_STATICAL(uint8_t value) noexcept
    {
        return static_cast<uint8_t>(value << STATICAL);
    }

    constexpr uint8_t GET_STRIPPED(uint8_t value) noexcept
    {
        return value & (1 << STRIPPED);
    }

    constexpr uint8_t SET_STRIPPED(uint8_t value) noexcept
    {
        return static_cast<uint8_t>(value << STRIPPED);
    }

}
