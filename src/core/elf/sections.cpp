#include <pwnit/core/elf/elf.hpp>

namespace pwnit::elf
{

Section::Section(const LIEF::ELF::Section &sec)
    : name(sec.name()),
      address(sec.virtual_address()),
      size(sec.size()),
      offset(sec.file_offset()) {}

Section::Section(const LIEF::ELF::Section *sec)
    : name(sec->name()),
      address(sec->virtual_address()),
      size(sec->size()),
      offset(sec->file_offset()) {}
   
decltype(Sections::sections)
Sections::load(const LIEF::ELF::Binary &binary)
{
    for (const auto &section : binary.sections()) {
        if (section.name().size() == 0) [[unlikely]]
            continue;
        
        this->sections.emplace_back(section);
    }

    return this->sections;
}

}
