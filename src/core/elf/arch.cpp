#include <pwnit/core/elf/elf.hpp>
#include <pwnit/utils/assert.hpp>

#include <capstone/capstone.h>

namespace pwnit::elf
{

Arch::operator cs_arch() const
{
    switch (static_cast<LIEF::ELF::ARCH>(value)) {
    case LIEF::ELF::ARCH::I386:
        return CS_ARCH_X86;
            
    case LIEF::ELF::ARCH::X86_64:
        return CS_ARCH_X86;
        
    case LIEF::ELF::ARCH::ARM:
        return CS_ARCH_ARM;
        
    case LIEF::ELF::ARCH::AARCH64:
        return CS_ARCH_ARM64;
        
    default:
        assert::fail(false, "Unsupported architecture");
    }
    
    return CS_ARCH_ALL;
}
    
const char *Arch::to_string() const
{
    return LIEF::ELF::to_string(
        static_cast<LIEF::ELF::ARCH>(this->value)
    );
}

}
