#pragma once

#include <pwnit/commands.hpp>

namespace pwnit::patch
{
    std::string patchelf(const commands::StartOptions &opt);
}
