#pragma once

#include <pwnit/utils/assert.hpp>

#include <capstone/arm.h>
#include <capstone/capstone.h>
#include <capstone/x86.h>

namespace pwnit::rop
{

using IsRet = bool (*)(const cs_insn&);

static
bool is_x86_ret(const cs_insn& instr)
{
    return instr.id == X86_INS_RET;
}

static
bool is_arm_ret(const cs_insn& instr)
{
    return instr.id == ARM_INS_BX &&
           instr.detail->arm.operands[0].type == ARM_OP_REG &&
           instr.detail->arm.operands[0].reg == ARM_REG_LR;
}

static
bool is_arm64_ret(const cs_insn& instr)
{
    return instr.id == ARM64_INS_RET;
}
    
static
IsRet get_is_ret(cs_arch arch)
{
    switch (arch) {
    case CS_ARCH_X86:
        return is_x86_ret;
        
    case CS_ARCH_ARM:
        return is_arm_ret;
        
    case CS_ARCH_ARM64:
        return is_arm64_ret;
        
    default:
        assert::fail(false, "Unsupported architecture");
    }

    return [] (const cs_insn &_)
    	{ return false; };
}

}
