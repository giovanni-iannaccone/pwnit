#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include <pwnit/core/elf/elf.hpp>
#include <capstone/capstone.h>

namespace pwnit::disassembler
{
    std::vector<cs_insn>
    disass(
        const elf::Section &sec,
        const std::span<const uint8_t> &content,
        cs_arch arch, cs_mode mode
    );
}
