#include <algorithm>
#include <cstdint>
#include <format>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/services/disassembler/disassembler.hpp>
#include <pwnit/utils/console.hpp>

#include <rop/rop.hpp>
#include <rop/utils.hpp>

#include <BS_thread_pool.hpp>

namespace pwnit::rop
{

struct GadgetFilter
{
    std::string_view mnemonic;
    std::string_view operands;
};

struct ScanContext
{
    int depth;
    int max_bytes;
    cs_arch arch;
    cs_mode mode;
    uint64_t address;
    std::span<const uint8_t> content;
    const IsEnding &is_ending;
    const GadgetFilter &filter;
};

using Gadget = std::vector<cs_insn>;
using Gadgets = std::vector<Gadget>;

static
GadgetFilter parse_filter(const std::string &search)
{
    const std::string_view value{search};

    const auto first = value.find_first_not_of(" \t");
    if (first == std::string_view::npos)
        return {};

    const auto last = value.find_last_not_of(" \t");
    const auto trimmed = value.substr(first, last - first + 1);

    const auto separator = trimmed.find_first_of(" \t");

    if (separator == std::string_view::npos)
        return {trimmed, {}};

    const auto operands = trimmed.find_first_not_of(" \t", separator);

    if (operands == std::string_view::npos)
        return {trimmed.substr(0, separator), {}};

    return {
        trimmed.substr(0, separator),
        trimmed.substr(operands)
    };
}

static inline
bool matches(
    const cs_insn &instruction,
    const GadgetFilter &filter
) {
    if (filter.mnemonic.empty())
        return true;

    if (filter.mnemonic != instruction.mnemonic)
        return false;

    return filter.operands.empty() ||
           filter.operands == instruction.op_str;
}

static inline
std::string gadget_text(
    std::span<const cs_insn> instructions
) {
    std::string text;

    for (const auto &instruction : instructions) {
        if (!text.empty())
            text += "; ";

        text += instruction.mnemonic;

        if (instruction.op_str[0] != '\0') {
            text += ' ';
            text += instruction.op_str;
        }
    }

    return text;
}

static inline
void print_gadget(
    std::span<const cs_insn> instructions,
    std::unordered_set<std::string> &seen
) {
    if (instructions.empty())
        return;

    const auto text = gadget_text(instructions);

    if (!seen.insert(text).second)
        return;

    const auto header =
        std::format("{:#x}: ", instructions.front().address);

    console::message(header, "{}", text);
}

static
void scan_start(
    const ScanContext &context,
    int start, Gadgets &results
) {
    const int content_size =
        static_cast<int>(context.content.size());

    const int remaining = content_size - start;
    const int length = std::min(remaining, context.max_bytes);

    const auto decoded = disassembler::disass(
        context.content.subspan(start, length),
        context.address + start,
        context.arch,
        context.mode
    );

    if (decoded.empty())
        return;

    if (!matches(decoded.front(), context.filter))
        return;

    Gadget gadget;
    gadget.reserve(context.depth);

    for (const auto &instruction : decoded) {
        if (static_cast<int>(gadget.size()) >= context.depth)
            break;

        if (!gadget.empty()) {
            const auto &previous = gadget.back();

            if (instruction.address !=
                previous.address + previous.size)
                break;
        }

        gadget.push_back(instruction);

        if (context.is_ending(instruction)) {
            results.push_back(std::move(gadget));
            return;
        }
    }
}

static inline
Gadgets scan_block(
    const ScanContext &context,
    int begin,
    int end
) {
    Gadgets results;

    for (int start = begin; start < end; ++start)
        scan_start(context, start, results);

    return results;
}

static
void scan(const ScanContext &context)
{
    BS::thread_pool pool;

    std::mutex mutex;
    Gadgets results;

    pool.detach_blocks(
        0,
        static_cast<int>(context.content.size()),
        [&](int begin, int end) {
            auto local = scan_block(context, begin, end);

            std::lock_guard lock(mutex);

            for (auto &gadget : local)
                results.push_back(std::move(gadget));
        }
    );

    pool.wait();

    std::unordered_set<std::string> seen;

    for (const auto &gadget : results)
        print_gadget(gadget, seen);
}

void gadgets(commands::RopOptions &opt)
{
    auto e = elf::Elf(opt.elf);
    const auto &[section, content] = e.get_section(".text");

    constexpr int max_instruction_size = 15;

    const GadgetFilter filter = parse_filter(opt.search);
    const IsEnding &is_ending = get_is_ending(e.arch);

    const ScanContext context{
        .depth = opt.depth,
        .max_bytes = opt.depth * max_instruction_size,
        .arch = e.arch,
        .mode = e.elf_class(),
        .address = section.address,
        .content = content,
        .is_ending = is_ending,
        .filter = filter
    };

    scan(context);
}

}
