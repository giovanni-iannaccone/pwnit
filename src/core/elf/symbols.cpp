#include <pwnit/core/elf/elf.hpp>

namespace pwnit::elf
{

Symbol::Symbol(const LIEF::Symbol * const sym) noexcept
    : name(sym->name()),
      address(sym->value()),
      size(sym->size()) {}
    
Symbol::Symbol(const LIEF::Symbol &sym) noexcept
    : name(sym.name()),
      address(sym.value()),
      size(sym.size()) {}

decltype(Symbols::symbols)
Symbols::load(const LIEF::ELF::Binary &binary)
{
    for (const auto &symbol : binary.symbols()) {
        if (symbol.name().size() == 0) [[unlikely]]
            continue;
        
        this->symbols.emplace_back(symbol);
    }

    return this->symbols;
}

}
