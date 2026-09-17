#include <optional>
#include <string_view>

#include <pwnit/core/arch/arch.hpp>
#include <pwnit/core/config/config.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/services/disassembler/disassembler.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>

#include <analysis/analysis.hpp>

#include <capstone/capstone.h>
#include <toml++/impl/array.hpp>

namespace pwnit::analysis
{

struct AnalysisContext
{
    elf::PLT plt;
    elf::Symbol sym;
    elf::Arch arch;
    cs_mode mode;
};

using Code = std::span<const uint8_t>;
    
static std::optional<std::string_view>
is_dangerous(const elf::PLT &plt, const cs_detail *detail)
{
    const auto func = plt[detail];
    const auto functions = config::Config::instance().dangerous_functions;

    for (const auto &f: *functions)
        if (f.is_string() && *f.as_string() == func)
            return func;

    return std::nullopt;
}

void print_if_dangerous(const elf::PLT &plt, const cs_insn &instr)
{
    const auto func = is_dangerous(plt, instr.detail);
    if (func.has_value())
        console::warn("call to {} at {}", func.value(), instr.address);
}
    
static void check_dangerous_function(
    const AnalysisContext &ctx, const Code &code
) {
    const auto asmb =
        disassembler::disass(code, ctx.sym.address, ctx.arch, ctx.mode);

    const auto is_call = arch::get_is_call(ctx.arch);
    console::info("xrefs for {}", ctx.sym.name);

    for (const auto &instr : asmb)
        if (is_call(instr))
            print_if_dangerous(ctx.plt, instr);
}

void analyze(commands::AnalysisOptions &opt)
{
    elf::Elf e {opt.file};
    assert::fail(
        !e.stripped(), "Functionality still not supported for stripped binaries"
    );
        
    for (const auto &sym: e.load_symbols()) {
        const auto && [_, content] = e.get_symbol(sym.name);

        const AnalysisContext ctx {
            e.get_plt(), sym, e.arch, e.elf_class()
        };
        
        check_dangerous_function(ctx, content);
    }
}
    
}
