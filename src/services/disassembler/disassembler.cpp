#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include <pwnit/core/elf/elf.hpp>
#include <pwnit/utils/assert.hpp>

#include <capstone/capstone.h>

namespace pwnit::disassembler
{

std::vector<cs_insn>
disass(
    const elf::Section &sec,
    const std::span<const uint8_t> &code,
    cs_arch arch, cs_mode mode
) {
    
    csh handle;
    
    assert::fail(
    	cs_open(arch, mode, &handle) == CS_ERR_OK,
        "Capstone initialization failed"
    );
        
    const uint8_t* ptr = code.data();
    size_t size = code.size();
    uint64_t current_address = sec.address;
    
    cs_insn* insn = cs_malloc(handle);
    std::vector<cs_insn> instructions;
    
    if (insn == nullptr) {
        cs_close(&handle);
        assert::fail(false, "Couldn't allocate memory for capstone");
        return {};
    }

    while (cs_disasm_iter(handle, &ptr, &size, &current_address, insn))
        instructions.push_back(*insn);

    cs_free(insn, 1);
    cs_close(&handle);

    return instructions;
}

}
