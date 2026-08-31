#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/utils/console.hpp>

#include <check/check.hpp>

namespace pwnit::checksec
{
    
static inline
void handle_json(elf::Elf &e, bool sections, bool symbols)
{
    if (sections) e.load_sections();
    if (symbols) e.load_symbols();

    console::log("{}", e.to_json().dump(4));
}

static inline
void print_generic_data(const elf::Elf &e)
{
    console::log("Arch: {}\t{}\tLinking: {}",
        e.arch.to_string(),
        e.stripped() ? "Stripped" : "Not stripped",
        e.statical() ? "Dynamic": "Static"
    );
    
    console::success("Entry point: {:#x}", e.entry);
}

static inline
void print_relro(uint8_t relro)
{
    switch (relro) {
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
}
    
static inline
void print_elf_sec_measures(const elf::Elf &e)
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

    print_relro(e.relro());
    console::log("");
}

static inline
void print_section(const elf::Section &sec)
{
    console::success("{}:", sec.name);
    console::log("\taddr: {:#x}\toffset: {}\tsize: {}\n", sec.address, sec.offset, sec.size);
}

static inline
void print_sections(elf::Elf &e)
{
    console::info("SECTIONS:");
    
    for (auto &&sec: e.load_sections())
        print_section(sec);
}

static inline
void print_symbol(const elf::Symbol &sym)
{
    console::success("{}:", sym.name);
    console::log("\taddr: {:#x}\tsize: {}\n", sym.address, sym.size);
}

static inline
void print_symbols(elf::Elf &e)
{
    console::info("SYMBOLS:");
    
    for (auto &&sym: e.load_symbols())
        print_symbol(sym);
}

void checksec(const commands::CheckOptions &opt)
{
    elf::Elf e {opt.file};

    if (opt.json) {
        handle_json(e, opt.sections, opt.symbols);
        return;
    }
    
    console::log("[ {} ]\n", opt.file);

    print_generic_data(e);
    print_elf_sec_measures(e);
    
    if (opt.sections) print_sections(e);    
    if (opt.symbols) print_symbols(e);
}

};
