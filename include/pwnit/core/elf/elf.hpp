#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <LIEF/ELF.hpp>
#include <nlohmann/json.hpp>

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

constexpr std::string_view to_string(RELRO relro)
{
    switch (relro) {
        case RELRO::NONE:    return "none";
        case RELRO::PARTIAL: return "partial";
        case RELRO::FULL:    return "full";
    }

    return "unknown";
}

struct Arch
{
    uint8_t value {};
    const char *to_string() const;
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

struct SecurityMeasures
{
private:
    uint8_t metadata;

public:
    SecurityMeasures() = default;
    SecurityMeasures(const LIEF::ELF::Binary &binary);

    uint8_t canary() const noexcept;
    uint8_t nx() const noexcept;
    uint8_t pie() const noexcept;
    uint8_t relro() const noexcept;
    uint8_t statical() const noexcept;
    uint8_t stripped() const noexcept;
   
    void set_canary(uint8_t value) noexcept;
    void set_nx(uint8_t value) noexcept;
    void set_pie(uint8_t value) noexcept;
    void set_relro(uint8_t value) noexcept;
    void set_statical(uint8_t value) noexcept;
    void set_stripped(uint8_t value) noexcept;
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Arch, value)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Section, name, address, size, offset)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Symbol, name, address, size)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sections, sections)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Symbols, symbols)

struct Elf
{
protected:
    struct Impl;
    std::unique_ptr<Impl> impl;

    SecurityMeasures secmes;
    
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
        return secmes.canary();
    }

    uint8_t nx() const noexcept
    {
        return secmes.nx();
    }

    uint8_t pie() const noexcept
    {
        return secmes.pie();
    }

    uint8_t relro() const noexcept
    {
        return secmes.relro();
    }

    uint8_t stripped() const noexcept
    {
        return secmes.stripped();
    }

    uint8_t statical() const noexcept
    {
        return secmes.statical();
    }

    nlohmann::json to_json() const;
};

}
