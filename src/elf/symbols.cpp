#include <pwnit/elf/elf.hpp>

namespace pwnit::elf
{
    
decltype(Symbols::symbols)
Symbols::load(const LIEF::ELF::Binary &binary)
{
    for (const auto &symbol : binary.symbols()) {
        if (symbol.name().size() == 0) [[unlikely]]
            continue;
        
        this->symbols.emplace_back(
            symbol.name(),
            symbol.value(),
            symbol.size()
        );
    }

    return this->symbols;
}

}
