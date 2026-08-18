#include <pwnit/core/elf/elf.hpp>
#include <pwnit/core/elf/utils.hpp>
#include <pwnit/core/utils/utils.hpp>

#include <LIEF/ELF.hpp>

namespace pwnit::elf
{

static
bool has_bind_now(const LIEF::ELF::Binary &binary)
{
    if (binary[LIEF::ELF::DynamicEntry::TAG::BIND_NOW])
        return true;

    if (const auto* entry =
        binary[LIEF::ELF::DynamicEntry::TAG::FLAGS]) {
        
        if (const auto* flags =
            entry->cast<LIEF::ELF::DynamicEntryFlags>()) {
            if (flags->has(LIEF::ELF::DynamicEntryFlags::FLAG::BIND_NOW))
                return true;
        }
    }
    
    if (const auto* entry =
        binary[LIEF::ELF::DynamicEntry::TAG::FLAGS_1]) {
        
        if (const auto* flags =
            entry->cast<LIEF::ELF::DynamicEntryFlags>()) {
            if (flags->has(LIEF::ELF::DynamicEntryFlags::FLAG::NOW))
                return true;
        }
    }
    
    return false;
}
    
static
RELRO has_relro(const LIEF::ELF::Binary &binary)
{
    const bool has_relro =
        binary.has(LIEF::ELF::Segment::TYPE::GNU_RELRO);
    
    if (!has_relro)
        return RELRO::NONE;
    
    if (has_bind_now(binary))
        return RELRO::FULL;
    
    return RELRO::PARTIAL;
}
    
static inline
uint8_t extract_metadata(const LIEF::ELF::Binary &binary)
{
    return
        SET_CANARY(binary.has_dynamic_symbol("__stack_chk_fail")) |
        SET_NX(binary.has_nx()) |
        SET_PIE(binary.is_pie()) |
        SET_RELRO(has_relro(binary)) |
        SET_STATICAL(binary.has_interpreter()) |
        SET_STRIPPED(!binary.has_section(".symtab"));
}   

struct Elf::Impl
{
    std::unique_ptr<LIEF::ELF::Binary> binary;
};
    
Elf::Elf(const std::filesystem::path &filepath, bool die_on_error)
{
    this->impl = std::make_unique<Elf::Impl>();
    this->impl->binary = LIEF::ELF::Parser::parse(filepath.string());

    if (die_on_error) {
        utils::assert_fail(this->impl->binary != nullptr,
            "Failed to parse ELF: {}", filepath.string()
        );
    } else if (this->impl->binary == nullptr) {
        this->valid = false;
        return;
    }
    
    const auto &binary = *this->impl->binary;
    
    this->arch.value = static_cast<uint8_t>(binary.header().machine_type());
    this->entry = binary.entrypoint();

    this->metadata = extract_metadata(binary);
    this->valid = true;
}

Elf::~Elf() = default;

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
    
}
