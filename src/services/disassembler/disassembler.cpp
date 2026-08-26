#include <cstdint>
#include <span>
#include <vector>

#include <pwnit/core/elf/elf.hpp>
#include <pwnit/services/disassembler/disassembler.hpp>
#include <pwnit/utils/assert.hpp>

#include <capstone/capstone.h>

namespace pwnit::disassembler
{

static inline
csh init_handle(cs_arch arch, cs_mode mode)
{
    csh handle;
    
    assert::fail(
        cs_open(arch, mode, &handle) == CS_ERR_OK,
        "Capstone initialization failed"
    );
        
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    return handle;
}
    
std::vector<cs_insn>
disass(
    const std::span<const uint8_t> &code,
    uint64_t address,
    cs_arch arch, cs_mode mode
) {

    csh handle = init_handle(arch, mode);
    
    const uint8_t* ptr = code.data();
    size_t size = code.size();
    
    cs_insn* insn = cs_malloc(handle);
    std::vector<cs_insn> instructions;
    
    if (insn == nullptr) {
        cs_close(&handle);
        assert::fail(false, "Couldn't allocate memory for capstone");
        return {};
    }

    while (cs_disasm_iter(handle, &ptr, &size, &address, insn))
        instructions.emplace_back(*insn);

    cs_free(insn, 1);
    cs_close(&handle);

    return instructions;
}

}
