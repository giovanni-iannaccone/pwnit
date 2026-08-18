#pragma once

#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>

namespace pwnit::checksec {
    
    void checksec(const commands::CheckOptions &opt);
}
