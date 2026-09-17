#include <cctype>
#include <string_view>

#include <capstone/capstone.h>

namespace pwnit::arch 
{
    
bool mnemonic_is(
    const cs_insn &instr,
    std::string_view mnemonic
) {
    std::string_view current {instr.mnemonic};

    if (current.size() != mnemonic.size())
        return false;

    for (size_t i = 0; i < current.size(); ++i) {
        const auto a =
            static_cast<unsigned char>(current[i]);

        const auto b =
            static_cast<unsigned char>(mnemonic[i]);

        if (std::tolower(a) != std::tolower(b))
            return false;
    }

    return true;
}

}
