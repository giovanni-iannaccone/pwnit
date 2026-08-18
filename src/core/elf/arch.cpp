#include <pwnit/elf/elf.hpp>

namespace pwnit::elf
{
    
const char *Arch::to_string()
{
    return LIEF::ELF::to_string(
        static_cast<LIEF::ELF::ARCH>(this->value)
    );
}

}
