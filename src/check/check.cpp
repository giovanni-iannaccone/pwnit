#include <pwnit/check/check.hpp>
#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/utils/console.hpp>

namespace pwnit::checksec
{
    
static inline void log_section(elf::Section &sec)
{
    console::success("{}:", sec.name);
    console::log("\taddr: {:#x}\toffset: {}\tsize: {}\n", sec.address, sec.offset, sec.size);
}

static inline void log_symbol(elf::Symbol &sym)
{
    console::success("{}:", sym.name);
    console::log("\taddr: {:#x}\tsize: {}\n", sym.address, sym.size);
}

static inline void print_generic_data(elf::Elf &e)
{
    console::log("Arch: {}\t{}\tLinking: {}",
        e.arch.to_string(),
        e.stripped() ? "Stripped" : "Not stripped",
        e.statical() ? "Dynamic": "Static"
    );
    
    console::success("Entry point: {:#x}", e.entry);
}

static inline void print_elf_sec_measures(elf::Elf &e)
{
    if (e.nx())
        console::success("NX enabled");
    else
        console::error("NX disabled");

    if (e.pie())
        console::success("PIE enabled");
    else
        console::error("No PIE");

    if (e.canary())
        console::success("Canary found");
    else
        console::error("Canary not found");

    switch (e.relro()) {
    case elf::RELRO::NONE:
        console::error("No RELRO");
        break;
    case elf::RELRO::PARTIAL:
        console::warn("Partial RELRO");
        break;
    case elf::RELRO::FULL:
        console::success("Full RELRO");
        break;
    default:
        console::error("WHAT ?");
    }

    console::log("");
}

static inline void print_sections(elf::Elf &e)
{
    console::info("SECTIONS:");
    
    for (auto &&sec: e.load_sections())
        log_section(sec);
}

static inline void print_symbols(elf::Elf &e)
{
    console::info("SYMBOLS:");
    
    for (auto &&sym: e.load_symbols())
        log_symbol(sym);
}

void checksec(const commands::CheckOptions &opt)
{
    elf::Elf e {opt.file};

    console::log("[ {} ]\n", opt.file);

    print_generic_data(e);    
    print_elf_sec_measures(e);
    
    if (opt.sections) print_sections(e);
    
    if (opt.symbols) print_symbols(e);
}

};
