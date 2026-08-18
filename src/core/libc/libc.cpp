#include <pwnit/config/config.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/utils/console.hpp>

#include <curl/curl.h>
#include <LIEF/ELF.hpp>
#include <LIEF/ELF/Binary.hpp>

namespace pwnit::libc
{

void Libc::print_debug_info() const noexcept
{
    console::info("Libc info:");
    console::info(
        "soname: {}, version: {}\tbuild id: {}\t",
        this->soname, this->version, this->build_id
    ); 
}
    
static std::string get_build_id(const std::unique_ptr<LIEF::ELF::Binary> &binary)
{
    const LIEF::ELF::Note* note =
        binary->get(LIEF::ELF::Note::TYPE::GNU_BUILD_ID);

    if (note == nullptr)
        return "";

    std::ostringstream ss;

    for (uint8_t byte : note->description()) {
        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << static_cast<unsigned>(byte);
    }

    return ss.str();
}

static std::string get_version(const std::unique_ptr<LIEF::ELF::Binary> &binary)
{
    std::string highest;
    
    for (const auto& definition : binary->symbols_version_definition()) {
        for (const auto& aux : definition.symbols_aux()) {
            const std::string& name = aux.name();
            
            if (name.rfind("GLIBC_", 0) == 0)
                if (name > highest)
                    highest = name;
        }
    }

    return highest;
}

Libc identify(const std::string &path)
{
    const auto binary = LIEF::ELF::Parser::parse(path);

    const std::string build_id = get_build_id(binary);
    const std::string version = get_version(binary);
    
    return Libc {
        .build_id = build_id,
        .soname = path,
        .version = version
    };
}
    
}
