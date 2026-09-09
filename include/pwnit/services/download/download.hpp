#pragma once

#include <string>

#include <pwnit/core/libc/libc.hpp>

namespace pwnit::download
{        
    std::pair<std::string, std::string>
    download(const libc::Libc &lib);

    bool unstrip_libc(const libc::Libc &libc);
    bool unstrip_libc(const libc::Libc &libc, std::string &url);
}
