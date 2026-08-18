#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <pwnit/core/elf/utils.hpp>

#include <LIEF/ELF.hpp>

namespace pwnit::elf
{

constexpr auto DONT_DIE_ON_ERR = false;
constexpr auto DIE_ON_ERR = true;
    
enum RELRO: uint8_t
{
    NONE = 0,
    PARTIAL = 1,
    FULL = 2
};
    
struct Arch
{
    uint8_t value {};
    const char *to_string();
};
    
struct Section
{
    std::string name;
    uint64_t address {};
    uint64_t size {};
    uint64_t offset {};
};

struct Sections
{
    std::vector<Section> sections;
    decltype(Sections::sections) load(const LIEF::ELF::Binary &binary);
};

struct Symbol
{
    std::string name;
    uint64_t address {};
    uint64_t size {};
};

struct Symbols
{
    std::vector<Symbol> symbols;
    decltype(Symbols::symbols) load(const LIEF::ELF::Binary &binary);
};
    
struct Elf
{
protected:
    struct Impl;
    std::unique_ptr<Impl> impl;

    uint8_t metadata {};
    
public:
    bool valid = false;
    
    Arch arch {};
    uint64_t entry {};
    
    Sections sections {};
    Symbols symbols {};
    
    Elf() = default;
    Elf(const std::filesystem::path &filepath, bool die_on_error = true);

    ~Elf();

    Elf(Elf &&) noexcept;
    Elf &operator=(Elf &&) noexcept;

    Elf(const Elf &) = delete;
    Elf &operator=(const Elf &) = delete;

    decltype(Sections::sections) load_sections();
    decltype(Symbols::symbols) load_symbols();

    bool has_symbol(const std::string& name) const;
    bool has_dynamic_symbol(const std::string& name) const;
    bool has_section(const std::string& name) const;

    bool is_libc() const;
    bool is_loader() const;

    const std::string interpreter() const noexcept;
    
    uint8_t canary() const noexcept
    {
        return GET_CANARY(this->metadata);
    }

    uint8_t nx() const noexcept
    {
        return GET_NX(this->metadata);
    }

    uint8_t pie() const noexcept
    {
        return GET_PIE(this->metadata);
    }

    uint8_t relro() const noexcept
    {
        return GET_RELRO(this->metadata);
    }

    uint8_t stripped() const noexcept
    {
        return GET_STRIPPED(this->metadata);
    }

    uint8_t statical() const noexcept
    {
        return GET_STATICAL(this->metadata);
    }
};

}
