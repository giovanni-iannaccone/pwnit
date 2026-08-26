#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/services/disassembler/disassembler.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/ringbuffer.hpp>

#include <rop/rop.hpp>
#include <rop/utils.hpp>

namespace pwnit::rop
{

using IBuffer = utils::RingBuffer<cs_insn>;

static
bool contains_instruction(
    const std::vector<cs_insn> &instructions,
    size_t begin, size_t end,
    const std::string &search
) {
    for (size_t i = begin; i <= end; i++)
        if (std::string{instructions[i].mnemonic}.contains(search))
            return true;

    return false;
}

static
void print_gadget(IBuffer &gadget, bool only_first = false)
{
    if (gadget.empty())
        return;

    if (only_first) {
        std::string gadget_str;

        for (const auto &instr : gadget)
            gadget_str += std::format("{} {}; ", instr.mnemonic, instr.op_str);

        const std::string header =
            std::format("{:#x}: ", gadget.front().address);

        console::message(
            header, "{}", gadget_str
        );
        return;
    }

    while (!gadget.empty()) {
        std::string gadget_str;

        for (const auto &instr : gadget)
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
void print_gadget(
    const std::vector<cs_insn> &instructions,
    size_t begin, size_t end,
    bool only_first = false
) {
    IBuffer gadget {end - begin + 1};

    for (size_t i = begin; i <= end; i++)
        gadget.push(instructions[i]);

    print_gadget(gadget, only_first);
}

static
void print_gadgets(
    const std::vector<cs_insn> &instructions, int depth, const IsRet &isret
) {
    for (size_t ret = 0; ret < instructions.size(); ret++) {
        if (!isret(instructions[ret]))
            continue;

        const size_t first =
            ret >= static_cast<size_t>(depth - 1)
                ? ret - static_cast<size_t>(depth - 1)
                : 0;

        for (size_t begin = first; begin <= ret; begin++)
            print_gadget(instructions, begin, ret);
    }
}

static
void print_filtered_gadgets(
    const std::vector<cs_insn> &instructions, int depth,
    const std::string &search, const IsRet &isret
) {
    for (size_t ret = 0; ret < instructions.size(); ret++) {
        if (!isret(instructions[ret]))
            continue;

        const size_t first =
            ret >= static_cast<size_t>(depth - 1)
                ? ret - static_cast<size_t>(depth - 1)
                : 0;

        for (size_t begin = first; begin <= ret; begin++) {
            if (std::string{instructions[begin].mnemonic} != search)
                continue;

            print_gadget(instructions, begin, ret, true);
        }
    }
}

void gadgets(commands::RopOptions &opt)
{
    auto &&e = elf::Elf(opt.elf);
    const auto && [section, content] = e.get_section(".text");
    
    const auto instructions =
        disassembler::disass(
            content, section.address,
            e.arch, e.elf_class()
        );

    const IsRet &is_ret = get_is_ret(e.arch);

    if (opt.search.empty())
        print_gadgets(instructions, opt.depth, is_ret);
    else
        print_filtered_gadgets(
            instructions, opt.depth, opt.search, is_ret
        );
}

}
