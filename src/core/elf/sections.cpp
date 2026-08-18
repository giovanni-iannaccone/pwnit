#include <pwnit/elf/elf.hpp>

namespace pwnit::elf
{
    
decltype(Sections::sections)
Sections::load(const LIEF::ELF::Binary &binary)
{
    for (const auto &section : binary.sections()) {
        if (section.name().size() == 0) [[unlikely]]
            continue;
        
        this->sections.emplace_back(
            section.name(),
            section.virtual_address(),
            section.size(),
            section.file_offset()
        );
    }

    return this->sections;
}

}
