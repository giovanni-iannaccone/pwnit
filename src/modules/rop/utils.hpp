#pragma once

#include <cctype>
#include <string_view>

#include <pwnit/utils/assert.hpp>

#include <capstone/capstone.h>

namespace pwnit::rop
{

using IsEnding = bool (*)(const cs_insn&);

static
bool mnemonic_is(
    const cs_insn &instr,
    std::string_view mnemonic
)
{
    std::string_view current {instr.mnemonic};

    if (current.size() != mnemonic.size())
        return false;

    for (size_t i = 0; i < current.size(); ++i) {
        const auto a =
            static_cast<unsigned char>(current[i]);

        const auto b =
            static_cast<unsigned char>(mnemonic[i]);

        if (std::tolower(a) != std::tolower(b))
            return false;
    }

    return true;
}

static
bool is_x86_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "ret") ||
        mnemonic_is(instr, "retf") ||
        mnemonic_is(instr, "retfq") ||
        mnemonic_is(instr, "iret") ||
        mnemonic_is(instr, "iretd") ||
        mnemonic_is(instr, "iretq") ||
        mnemonic_is(instr, "sysret") ||
        mnemonic_is(instr, "sysretq") ||
        mnemonic_is(instr, "syscall") ||
        mnemonic_is(instr, "sysenter") ||
        mnemonic_is(instr, "sysexit"))
        return true;

    if (mnemonic_is(instr, "jmp") ||
        mnemonic_is(instr, "call"))
        return true;

    if (mnemonic_is(instr, "int") ||
        mnemonic_is(instr, "int1") ||
        mnemonic_is(instr, "int3"))
        return true;

    return false;
}

static
bool is_arm_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "bx")) {
        std::string_view op {instr.op_str};
        return op == "lr";
    }

    if (mnemonic_is(instr, "blx"))
        return true;

    if (mnemonic_is(instr, "mov")) {

        std::string_view op {instr.op_str};

        if (op == "pc, lr")
            return true;
    }

    if (mnemonic_is(instr, "pop")) {

        std::string_view op {instr.op_str};

        if (op.find("pc") != std::string_view::npos)
            return true;
    }

    if (mnemonic_is(instr, "ldm") ||
        mnemonic_is(instr, "ldmia") ||
        mnemonic_is(instr, "ldmfd") ||
        mnemonic_is(instr, "ldmdb") ||
        mnemonic_is(instr, "ldmea")) {

        std::string_view op {instr.op_str};

        if (op.find("pc") != std::string_view::npos)
            return true;
    }

    if (mnemonic_is(instr, "b") ||
        mnemonic_is(instr, "bl"))
        return true;

    return false;
}

static
bool is_arm64_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "ret") ||
        mnemonic_is(instr, "eret") ||
        mnemonic_is(instr, "drps"))
        return true;

    if (mnemonic_is(instr, "br") ||
        mnemonic_is(instr, "blr"))
        return true;

    return false;
}

static
bool is_mips_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "jr")) {

        std::string_view op {instr.op_str};

        if (op.find("$ra") != std::string_view::npos ||
            op.find("ra") != std::string_view::npos)
            return true;

        return true;
    }

    if (mnemonic_is(instr, "jalr"))
        return true;

    if (mnemonic_is(instr, "j") ||
        mnemonic_is(instr, "jal"))
        return true;

    return false;
}

static
bool is_ppc_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "blr") ||
        mnemonic_is(instr, "bctr") ||
        mnemonic_is(instr, "bctrl") ||
        mnemonic_is(instr, "bclr") ||
        mnemonic_is(instr, "bclrl"))
        return true;

    if (mnemonic_is(instr, "b") ||
        mnemonic_is(instr, "ba") ||
        mnemonic_is(instr, "bl") ||
        mnemonic_is(instr, "bla"))
        return true;

    return false;
}

static
bool is_sparc_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "ret") ||
        mnemonic_is(instr, "retl") ||
        mnemonic_is(instr, "jmpl"))
        return true;

    if (mnemonic_is(instr, "ba") ||
        mnemonic_is(instr, "call"))
        return true;

    return false;
}

static
bool is_sysz_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "br") ||
        mnemonic_is(instr, "bcr") ||
        mnemonic_is(instr, "bsm") ||
        mnemonic_is(instr, "bassm") ||
        mnemonic_is(instr, "brc") ||
        mnemonic_is(instr, "brasl"))
        return true;

    return false;
}

static
bool is_riscv_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "ret"))
        return true;

    if (mnemonic_is(instr, "jalr")) {

        std::string_view op {instr.op_str};

        if (op.find("ra") != std::string_view::npos)
            return true;

        return true;
    }

    if (mnemonic_is(instr, "j") ||
        mnemonic_is(instr, "jr"))
        return true;

    return false;
}

static
bool is_generic_ending(const cs_insn &instr)
{
    if (mnemonic_is(instr, "ret") ||
        mnemonic_is(instr, "retf") ||
        mnemonic_is(instr, "iret") ||
        mnemonic_is(instr, "iretq") ||
        mnemonic_is(instr, "sysret") ||
        mnemonic_is(instr, "eret"))
        return true;

    if (mnemonic_is(instr, "jmp") ||
        mnemonic_is(instr, "call") ||
        mnemonic_is(instr, "br") ||
        mnemonic_is(instr, "blr") ||
        mnemonic_is(instr, "bx"))
        return true;

    return false;
}

static
IsEnding get_is_ending(cs_arch arch)
{
    switch (arch) {

    case CS_ARCH_X86:
        return is_x86_ending;

    case CS_ARCH_ARM:
        return is_arm_ending;

    case CS_ARCH_ARM64:
        return is_arm64_ending;

    case CS_ARCH_MIPS:
        return is_mips_ending;

    case CS_ARCH_PPC:
        return is_ppc_ending;

    case CS_ARCH_SPARC:
        return is_sparc_ending;

    case CS_ARCH_SYSZ:
        return is_sysz_ending;

    case CS_ARCH_RISCV:
        return is_riscv_ending;

    default:
        return is_generic_ending;
    }
}

}
