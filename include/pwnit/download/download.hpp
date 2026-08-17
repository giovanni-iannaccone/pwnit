#pragma once

#include <pwnit/libc/libc.hpp>

namespace pwnit::download
{

std::string libc(libc::Libc libc);
std::string loader(libc::Libc libc);

}
