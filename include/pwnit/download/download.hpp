#pragma once

#include <pwnit/libc/libc.hpp>

namespace pwnit::download
{

std::string from_libc_db(libc::Libc libc);

}
