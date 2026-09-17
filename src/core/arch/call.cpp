#include <pwnit/core/arch/arch.hpp>
#include <capstone/capstone.h>

namespace pwnit::arch
{

static
bool is_x86_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "call");
}

static
bool is_arm_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "bl") ||
           mnemonic_is(instr, "blx");
}

static
bool is_arm64_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "bl") ||
           mnemonic_is(instr, "blr");
}

static
bool is_mips_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "jal") ||
           mnemonic_is(instr, "jalr");
}

static
bool is_ppc_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "bl") ||
           mnemonic_is(instr, "bla") ||
           mnemonic_is(instr, "bctrl");
}

static
bool is_sparc_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "call") ||
           mnemonic_is(instr, "jmpl");
}

static
bool is_sysz_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "brasl") ||
           mnemonic_is(instr, "bas") ||
           mnemonic_is(instr, "basr");
}

static
bool is_riscv_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "call") ||
           mnemonic_is(instr, "jal") ||
           mnemonic_is(instr, "jalr");
}

static
bool is_generic_call(const cs_insn &instr)
{
    return mnemonic_is(instr, "call") ||
           mnemonic_is(instr, "bl") ||
           mnemonic_is(instr, "blr") ||
           mnemonic_is(instr, "blx") ||
           mnemonic_is(instr, "jal") ||
           mnemonic_is(instr, "jalr");
}

IsCall get_is_call(cs_arch arch)
{
    switch (arch) {

    case CS_ARCH_X86:
        return is_x86_call;

    case CS_ARCH_ARM:
        return is_arm_call;

    case CS_ARCH_ARM64:
        return is_arm64_call;

    case CS_ARCH_MIPS:
        return is_mips_call;

    case CS_ARCH_PPC:
        return is_ppc_call;

    case CS_ARCH_SPARC:
        return is_sparc_call;

    case CS_ARCH_SYSZ:
        return is_sysz_call;

    case CS_ARCH_RISCV:
        return is_riscv_call;

    default:
        return is_generic_call;
    }
}

}
