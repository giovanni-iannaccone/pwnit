#pragma once

#include <pwnit/core/libc/libc.hpp>

namespace pwnit::download
{

constexpr auto DOWNLOAD_FAILED = 0;
    
std::string from_libc_db(libc::Libc libc);

}
