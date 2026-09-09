#pragma once

#include <pwnit/commands.hpp>

namespace pwnit::libc
{
    void info(const commands::LibcOptions &opt);
    void unstrip(const commands::LibcOptions &opt);
}
