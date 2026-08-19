#pragma once

#include <pwnit/commands.hpp>

namespace pwnit::start
{
    void find_binaries(commands::StartOptions &opt);
    void print_binaries(const commands::StartOptions &opt);

    void start(commands::StartOptions &opt);
}
