#include <pwnit/core/elf/elf.hpp>

namespace pwnit::elf
{
    
const char *Arch::to_string() const
{
    return LIEF::ELF::to_string(
        static_cast<LIEF::ELF::ARCH>(this->value)
    );
}

}
