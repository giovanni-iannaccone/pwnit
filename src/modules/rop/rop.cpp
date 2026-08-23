#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/services/disassembler/disassembler.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/ringbuffer.hpp>
#include <rop/rop.hpp>

#include <capstone/arm.h>
#include <capstone/capstone.h>
#include <capstone/mips.h>
#include <capstone/riscv.h>
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

using IBuffer = utils::RingBuffer<cs_insn>;
    
static
void print_gadget(IBuffer &gadget)
{
    while (!gadget.empty()) {
        std::string gadget_str;
        
        for (const auto &instr: gadget)
            gadget_str += std::format("{} {}; ", instr.mnemonic, instr.op_str);
        
        const std::string header =
            std::format("{:#x}: ", gadget.front().address);

        gadget.pop();
        console::message(
            header, "{}", gadget_str
        );
    }
}
    
static
void print_filtered_gadgets(
    const std::vector<cs_insn> &instructions, int depth,
    const std::string &search, const IsRet &isret
) {
    IBuffer gadget {static_cast<size_t>(depth)};

    int found = 0;
    
    for (const auto &instr: instructions) {

        if (std::string{instr.mnemonic}.contains(search)) {
            found = depth;
            gadget.push(instr);
            continue;
        }
        
        if (found == 0)
            continue;
        
        found--;        
        gadget.push(instr);
        if (isret(instr))
            print_gadget(gadget);
    }
}

static
void print_gadgets(
    const std::vector<cs_insn> &instructions, int depth, const IsRet &isret
) {
    IBuffer gadget {static_cast<size_t>(depth)};
    
    for (const auto &instr: instructions) {
        gadget.push(instr);
        if (isret(instr))
            print_gadget(gadget);
    }
}
    
void gadgets(commands::RopOptions &opt)
{
    auto &&e = elf::Elf(opt.elf);
    const auto && [section, content] = e.get_section(".text");
    
    const auto instructions = disassembler::disass(content, section.address, e.arch, e.elf_class());

    const IsRet &is_ret = get_is_ret(e.arch);
    if (opt.search.empty())
        print_gadgets(instructions, opt.depth, is_ret);
    else
        print_filtered_gadgets(instructions, opt.depth, opt.search, is_ret);
}

}
