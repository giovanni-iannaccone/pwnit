#include <pwnit/core/elf/elf.hpp>
#include <pwnit/utils/assert.hpp>

#include <capstone/capstone.h>
#include <LIEF/ELF.hpp>
#include <LIEF/ELF/Header.hpp>

namespace pwnit::elf
{

struct Elf::Impl
{
    std::unique_ptr<LIEF::ELF::Binary> binary;
};
    
Elf::Elf(const std::filesystem::path &filepath, bool die_on_error)
{
    this->impl = std::make_unique<Elf::Impl>();
    this->impl->binary = LIEF::ELF::Parser::parse(filepath.string());

    if (die_on_error) {
        assert::fail(this->impl->binary != nullptr,
            "Failed to parse ELF: {}", filepath.string()
        );
    } else if (this->impl->binary == nullptr) {
        this->valid = false;
        return;
    }
    
    const auto &binary = *this->impl->binary;
    
    this->arch.value = static_cast<uint8_t>(binary.header().machine_type());
    this->entry = binary.entrypoint();

    this->secmes = SecurityMeasures {binary};
    this->valid = true;
}

Elf::~Elf() = default;

cs_mode Elf::elf_class() const
{
    const auto &binary = *this->impl->binary;
    auto eclass = binary.header().identity_class();

    if (eclass == LIEF::ELF::Header::CLASS::ELF32)
        return CS_MODE_32;

    return CS_MODE_64;
}
    
std::pair<Section, const std::span<const uint8_t>>
Elf::get_section(const std::string &name) const
{
    const auto &binary = *this->impl->binary;
    if (!binary.has_section(name))
        return {};

    const auto sec = binary.get_section(name);
    Section section {sec};

    return {std::move(section), sec->content()};
}

std::pair<Symbol, const std::span<const uint8_t>>
Elf::get_symbol(const std::string &name) const
{
    const auto &binary = *this->impl->binary;
    if (!binary.has_symbol(name))
        return {};

    const auto sym = binary.get_symbol(name);
    Symbol symbol {sym};

    auto content =
        binary.get_content_from_virtual_address(sym->value(), sym->size());

    return {std::move(symbol), content};
}
    
bool Elf::has_symbol(const std::string& name) const
{
    const auto &binary = *this->impl->binary;
    return binary.has_symbol(name);
}
    
bool Elf::has_dynamic_symbol(const std::string& name) const
{
    const auto &binary = *this->impl->binary;
    return binary.has_dynamic_symbol(name);
}
    
bool Elf::has_section(const std::string& name) const
{
    const auto &binary = *this->impl->binary;
    return binary.has_section(name);
}

bool Elf::is_libc() const
{
    const auto &binary = *this->impl->binary;
    
    if (binary.header().file_type() != LIEF::ELF::Header::FILE_TYPE::DYN)
        return false;
    
    if (!binary.has_dynamic_symbol("__libc_start_main"))
        return false;
    
    const auto* soname = binary.get(LIEF::ELF::DynamicEntry::TAG::SONAME);
    if (soname == nullptr)
        return false;
    
    const auto* library =
        soname->cast<LIEF::ELF::DynamicEntryLibrary>();
    
    return library != nullptr && library->name().starts_with("libc.so");
}

bool Elf::is_loader() const
{
    const auto &binary = *this->impl->binary;
    
    if (binary.header().file_type() != LIEF::ELF::Header::FILE_TYPE::DYN)
        return false;

    return
        binary.has_dynamic_symbol("_dl_start") ||
        binary.has_dynamic_symbol("_rtld_global") ||
        binary.has_dynamic_symbol("_dl_debug_state");
}

const std::string Elf::interpreter() const noexcept
{
    const auto &binary = *this->impl->binary;

    if (binary.has_interpreter())
        return binary.interpreter();

    return "";
}
    
decltype(Sections::sections)
Elf::load_sections()
{
    return this->sections.load(*this->impl->binary);
}

decltype(Symbols::symbols)
Elf::load_symbols()
{
    return this->symbols.load(*this->impl->binary);
}
    
nlohmann::json Elf::to_json() const
{
    return {
        {"arch", this->arch.to_string()},
        {"entry", std::format("{:#x}", this->entry)},
        {"sections", this->sections},
        {"symbols", this->symbols},
        {"canary", this->canary()},
        {"nx", this->nx()},
        {"pie", this->pie()},
        {"relro", elf::to_string(RELRO{this->relro()})},
        {"stripped", this->stripped()},
        {"statical", this->statical()}
    };
}

}
