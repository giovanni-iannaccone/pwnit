#pragma once

#include <string_view>
#include <capstone/capstone.h>

namespace pwnit::arch
{
    
using IsCall = bool (*)(const cs_insn&);
using IsEnding = bool (*)(const cs_insn&);

bool mnemonic_is(const cs_insn &instr, std::string_view mnemonic);
    
IsCall get_is_call(cs_arch arch);
IsEnding get_is_ending(cs_arch arch);

}
